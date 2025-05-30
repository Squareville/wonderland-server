#include "InstanceManager.h"
#include <string>
#include "Game.h"
#include "dServer.h"
#include "Logger.h"
#include "dConfig.h"
#include "CDClientDatabase.h"
#include "CDClientManager.h"
#include "CDZoneTableTable.h"
#include "MasterPackets.h"
#include "BitStreamUtils.h"
#include "eConnectionType.h"
#include "MessageType/Master.h"

#include "Start.h"

InstanceManager::InstanceManager(Logger* logger, const std::string& externalIP) {
	mLogger = logger;
	mExternalIP = externalIP;
	m_LastPort =
		GeneralUtils::TryParse<uint16_t>(Game::config->GetValue("world_port_start")).value_or(m_LastPort);
	m_LastInstanceID = LWOINSTANCEID_INVALID;
}

InstanceManager::~InstanceManager() {
	for (Instance* i : m_Instances) {
		delete i;
		i = nullptr;
	}
}

Instance* InstanceManager::GetInstance(LWOMAPID mapID, bool isFriendTransfer, LWOCLONEID cloneID) {
	LOG("Searching for an instance for mapID %i/%i", mapID, cloneID);
	Instance* instance = FindInstance(mapID, isFriendTransfer, cloneID);
	if (instance) return instance;

	// If we are shutting down, return a nullptr so a new instance is not created.
	if (m_IsShuttingDown) {
		LOG("Tried to create a new instance map/instance/clone %i/%i/%i, but Master is shutting down.",
			mapID,
			m_LastInstanceID + 1,
			cloneID);
		return nullptr;
	}
	//TODO: Update this so that the IP is read from a configuration file instead

	int softCap = 8;
	int maxPlayers = 12;

	if (mapID == 0) {
		softCap = 999;
		maxPlayers = softCap;
	} else {
		softCap = GetSoftCap(mapID);
		maxPlayers = GetHardCap(mapID);
	}

	uint32_t port = GetFreePort();
	instance = new Instance(mExternalIP, port, mapID, ++m_LastInstanceID, cloneID, softCap, maxPlayers);

	//Start the actual process:
	StartWorldServer(mapID, port, m_LastInstanceID, maxPlayers, cloneID);

	m_Instances.push_back(instance);

	if (instance) {
		LOG("Created new instance: %i/%i/%i with min/max %i/%i", mapID, m_LastInstanceID, cloneID, softCap, maxPlayers);
		return instance;
	} else LOG("Failed to create a new instance!");

	return nullptr;
}

bool InstanceManager::IsPortInUse(uint32_t port) {
	for (Instance* i : m_Instances) {
		if (i && i->GetPort() == port) {
			return true;
		}
	}

	return false;
}

uint32_t InstanceManager::GetFreePort() {
	uint32_t port = m_LastPort;
	std::vector<uint32_t> usedPorts;
	for (Instance* i : m_Instances) {
		usedPorts.push_back(i->GetPort());
	}

	std::sort(usedPorts.begin(), usedPorts.end());

	int portIdx = 0;
	while (portIdx < usedPorts.size() && port == usedPorts[portIdx]) {
		//increment by 3 since each instance uses 3 ports (instance, world-server, world-chat)
		port += 3;
		portIdx++;
	}

	return port;
}

void InstanceManager::AddPlayer(SystemAddress systemAddr, LWOMAPID mapID, LWOINSTANCEID instanceID) {
	Instance* inst = FindInstance(mapID, instanceID);
	if (inst) {
		Player player;
		player.addr = systemAddr;
		player.id = 0; //TODO: Update this to include the LWOOBJID of the player's character.
		inst->AddPlayer(player);
	}
}

void InstanceManager::RemovePlayer(SystemAddress systemAddr, LWOMAPID mapID, LWOINSTANCEID instanceID) {
	Instance* inst = FindInstance(mapID, instanceID);
	if (inst) {
		Player player;
		player.addr = systemAddr;
		player.id = 0; //TODO: Update this to include the LWOOBJID of the player's character.
		inst->RemovePlayer(player);
	}
}

std::vector<Instance*> InstanceManager::GetInstances() const {
	return m_Instances;
}

void InstanceManager::AddInstance(Instance* instance) {
	if (instance == nullptr) return;

	m_Instances.push_back(instance);
}

void InstanceManager::RemoveInstance(Instance* instance) {
	for (uint32_t i = 0; i < m_Instances.size(); ++i) {
		if (m_Instances[i] == instance) {
			instance->SetShutdownComplete(true);

			if (!Game::ShouldShutdown()) RedirectPendingRequests(instance);

			delete m_Instances[i];

			m_Instances.erase(m_Instances.begin() + i);

			break;
		}
	}
}

void InstanceManager::ReadyInstance(Instance* instance) {
	instance->SetIsReady(true);

	auto& pending = instance->GetPendingRequests();

	for (const auto& request : pending) {
		const auto& zoneId = instance->GetZoneID();

		LOG("Responding to pending request %llu -> %i (%i)", request, zoneId.GetMapID(), zoneId.GetCloneID());

		MasterPackets::SendZoneTransferResponse(
			Game::server,
			request.sysAddr,
			request.id,
			request.mythranShift,
			zoneId.GetMapID(),
			zoneId.GetInstanceID(),
			zoneId.GetCloneID(),
			instance->GetIP(),
			instance->GetPort()
		);
	}

	pending.clear();
}

void InstanceManager::RequestAffirmation(Instance* instance, const PendingInstanceRequest& request) {
	instance->GetPendingAffirmations().push_back(request);

	CBITSTREAM;

	BitStreamUtils::WriteHeader(bitStream, eConnectionType::MASTER, MessageType::Master::AFFIRM_TRANSFER_REQUEST);

	bitStream.Write(request.id);

	Game::server->Send(bitStream, instance->GetSysAddr(), false);

	LOG("Sent affirmation request %llu to %i/%i", request.id,
		static_cast<int>(instance->GetZoneID().GetMapID()),
		static_cast<int>(instance->GetZoneID().GetCloneID())
	);
}

void InstanceManager::AffirmTransfer(Instance* instance, const uint64_t transferID) {
	auto& pending = instance->GetPendingAffirmations();

	for (auto i = 0u; i < pending.size(); ++i) {
		const auto& request = pending[i];

		if (request.id != transferID) continue;

		const auto& zoneId = instance->GetZoneID();

		MasterPackets::SendZoneTransferResponse(
			Game::server,
			request.sysAddr,
			request.id,
			request.mythranShift,
			zoneId.GetMapID(),
			zoneId.GetInstanceID(),
			zoneId.GetCloneID(),
			instance->GetIP(),
			instance->GetPort()
		);

		pending.erase(pending.begin() + i);

		break;
	}
}

void InstanceManager::RedirectPendingRequests(Instance* instance) {
	const auto& zoneId = instance->GetZoneID();

	for (const auto& request : instance->GetPendingAffirmations()) {
		auto* in = Game::im->GetInstance(zoneId.GetMapID(), false, zoneId.GetCloneID());

		if (in && !in->GetIsReady()) // Instance not ready, make a pending request
		{
			in->GetPendingRequests().push_back(request);

			continue;
		}

		Game::im->RequestAffirmation(in, request);
	}
}

Instance* InstanceManager::GetInstanceBySysAddr(SystemAddress& sysAddr) {
	for (uint32_t i = 0; i < m_Instances.size(); ++i) {
		if (m_Instances[i] && m_Instances[i]->GetSysAddr() == sysAddr) {
			return m_Instances[i];
		}
	}

	return nullptr;
}

bool InstanceManager::IsInstanceFull(Instance* instance, bool isFriendTransfer) {
	if (!isFriendTransfer && instance->GetSoftCap() > instance->GetCurrentClientCount())
		return false;
	else if (isFriendTransfer && instance->GetHardCap() > instance->GetCurrentClientCount())
		return false;

	return true;
}

Instance* InstanceManager::FindInstance(LWOMAPID mapID, bool isFriendTransfer, LWOCLONEID cloneId) {
	for (Instance* i : m_Instances) {
		if (i && i->GetMapID() == mapID && i->GetCloneID() == cloneId && !IsInstanceFull(i, isFriendTransfer) && !i->GetIsPrivate() && !i->GetShutdownComplete() && !i->GetIsShuttingDown()) {
			return i;
		}
	}

	return nullptr;
}

Instance* InstanceManager::FindInstance(LWOMAPID mapID, LWOINSTANCEID instanceID) {
	for (Instance* i : m_Instances) {
		if (i && i->GetMapID() == mapID && i->GetInstanceID() == instanceID && !i->GetIsPrivate() && !i->GetShutdownComplete() && !i->GetIsShuttingDown()) {
			return i;
		}
	}

	return nullptr;
}

std::vector<Instance*> InstanceManager::FindInstancesByMapID(LWOMAPID mapID) {
	std::vector<Instance*> instances;
	for (Instance* instance : m_Instances) {
		if (instance && instance->GetMapID() == mapID) {
			instances.push_back(instance);
		}
	}

	return instances;
}

Instance* InstanceManager::FindInstanceWithPrivate(LWOMAPID mapID, LWOINSTANCEID instanceID) {
	for (Instance* i : m_Instances) {
		if (i && i->GetMapID() == mapID && i->GetInstanceID() == instanceID && !i->GetShutdownComplete() && !i->GetIsShuttingDown()) {
			return i;
		}
	}

	return nullptr;
}

Instance* InstanceManager::CreatePrivateInstance(LWOMAPID mapID, LWOCLONEID cloneID, const std::string& password) {
	auto* instance = FindPrivateInstance(password);

	if (instance != nullptr) {
		return instance;
	}

	if (m_IsShuttingDown) {
		LOG("Tried to create a new private instance map/instance/clone %i/%i/%i, but Master is shutting down.",
			mapID,
			m_LastInstanceID + 1,
			cloneID);
		return nullptr;
	}

	int maxPlayers = 999;

	uint32_t port = GetFreePort();
	instance = new Instance(mExternalIP, port, mapID, ++m_LastInstanceID, cloneID, maxPlayers, maxPlayers, true, password);

	//Start the actual process:
	StartWorldServer(mapID, port, m_LastInstanceID, maxPlayers, cloneID);

	m_Instances.push_back(instance);

	if (instance) return instance;
	else LOG("Failed to create a new instance!");

	return instance;
}

Instance* InstanceManager::FindPrivateInstance(const std::string& password) {
	for (auto* instance : m_Instances) {
		if (!instance) continue;

		if (!instance->GetIsPrivate()) {
			continue;
		}

		LOG("Password: %s == %s => %d", password.c_str(), instance->GetPassword().c_str(), password == instance->GetPassword());

		if (instance->GetPassword() == password) {
			return instance;
		}
	}

	return nullptr;
}

int InstanceManager::GetSoftCap(LWOMAPID mapID) {
	const CDZoneTable* zone = CDZoneTableTable::Query(mapID);

	// Default to 8 which is the cap for most worlds.
	return zone ? zone->population_soft_cap : 8;
}

int InstanceManager::GetHardCap(LWOMAPID mapID) {
	const CDZoneTable* zone = CDZoneTableTable::Query(mapID);

	// Default to 12 which is the cap for most worlds.
	return zone ? zone->population_hard_cap : 12;
}

void Instance::SetShutdownComplete(const bool value) {
	m_Shutdown = value;
}

bool Instance::GetShutdownComplete() const {
	return m_Shutdown;
}

void Instance::Shutdown() {
	CBITSTREAM;

	BitStreamUtils::WriteHeader(bitStream, eConnectionType::MASTER, MessageType::Master::SHUTDOWN);

	Game::server->Send(bitStream, this->m_SysAddr, false);

	LOG("Triggered world shutdown for zone/clone/instance %i/%i/%i", GetMapID(), GetCloneID(), GetInstanceID());
}
