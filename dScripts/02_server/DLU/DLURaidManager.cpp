#include "DLURaidManager.h"

#include <chrono>

#include "Character.h"
#include "TeamManager.h"
#include "ZoneInstanceManager.h"
#include "CharacterComponent.h"
#include "WorldPackets.h"
#include "ChatPackets.h"

void DLURaidManager::OnStartup(Entity* self) {
	
}

void DLURaidManager::OnUse(Entity* self, Entity* user) {
	auto* team = TeamManager::Instance()->GetTeam(user->GetObjectID());
	if (!team) {
		return;
	}

	bool teamSignedUp = false;

	for (const auto& playerObjectId : team->members) {
		for (const auto& leader : m_PlayerLeaders) {
			if (leader->GetObjectID() == playerObjectId) {
				teamSignedUp = true;
				break;
			}
		}
		if (teamSignedUp) break;
	}

	if (teamSignedUp) {
		Game::logger->Log("RaidManager", "Team of signing up user %s, is already signed up for the raid.", user->GetCharacter()->GetName().c_str());

		ChatPackets::SendSystemMessage(user->GetSystemAddress(), u"Your team is already signed up for the raid.");

		return;
	}
	
	m_PlayerLeaders.push_back(user);
	Game::logger->Log("RaidManager", "Added new team with leader %s adding them.", user->GetCharacter()->GetName().c_str());
}

void DLURaidManager::OnUpdate(Entity* self) {
	// get timestamp
	uint64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	if (m_Raid.m_RaidTime != 0) {
		// check if raid is over
		if (timestamp >= m_Raid.m_RaidTime) {
			// handle cancelling of prepare time
			this->EndPreperationStage();

			// raid is over, remove raid
			m_Raid = Raid();

			// delete ourselves
			EntityManager::Instance()->DestructEntity(self);
		}
	}
}

void DLURaidManager::EndPreperationStage() {
	// Move people over to the raid zone before we disappear
	for (const auto* player : m_PlayerLeaders) {
		auto team = TeamManager::Instance()->GetTeam(player->GetObjectID());
		
		auto playersToMove = std::vector<LWOOBJID>();
		playersToMove.push_back(player->GetObjectID()); // this is intended, just weird, it is a fallback for if a user is not in a team (not sure why you wouldn't be though
		
		if (team) {
			playersToMove = team->members;	
		}

		for (const auto playerObjectId : playersToMove) {
			auto playerEntity = EntityManager::Instance()->GetEntity(playerObjectId);

			if (!playerEntity) continue;

			ZoneInstanceManager::Instance()->RequestZoneTransfer(Game::server, m_Raid.m_RaidZone, 0, false, [playerEntity](bool mythranShift, uint32_t zoneID, uint32_t zoneInstance, uint32_t zoneClone, std::string serverIP, uint16_t serverPort) {
				const auto sysAddr = playerEntity->GetSystemAddress();

				if (playerEntity->GetCharacter()) {
					playerEntity->GetCharacter()->SetZoneID(zoneID);
					playerEntity->GetCharacter()->SetZoneInstance(zoneInstance);
					playerEntity->GetCharacter()->SetZoneClone(zoneClone);
					playerEntity->GetComponent<CharacterComponent>()->SetLastRocketConfig(u"");
				}

				playerEntity->GetCharacter()->SaveXMLToDatabase();

				WorldPackets::SendTransferToWorld(sysAddr, serverIP, serverPort, mythranShift);
				return;
			});
		}
	}
	
}

void DLURaidManager::SetRaid(Raid raid) {
	m_Raid = raid;
}

Raid DLURaidManager::GetRaid() {
	return m_Raid;
}

uint32_t DLURaidManager::GetTeamCount() {
	return m_PlayerLeaders.size();
}

uint32_t DLURaidManager::GetPlayerCount() {
	uint32_t rollingCount = 0;
	for (const auto* player : m_PlayerLeaders) {
		auto team = TeamManager::Instance()->GetTeam(player->GetObjectID());

		if (team) {
			rollingCount += team->members.size();
		}
	}

	return rollingCount;
}
