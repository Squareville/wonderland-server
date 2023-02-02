#include "SkunkEvent.h"

#include "Entity.h"
#include "EntityManager.h"
#include "GameMessages.h"
#include "Loot.h"
#include "RebuildComponent.h"
#include "RenderComponent.h"
#include "Zone.h"
#include "ZorilloContants.h"

#include "dZoneManager.h"

bool SkunkEvent::IsInvasionActive(Entity* self) {
	auto state = GetZoneState(self);
	Game::logger->Log("SkunkEvent", "is invasion active %i", (state == SkunkEventState::ZoneStateNoInfo || state == SkunkEventState::ZoneStateNoInvasion || state == SkunkEventState::ZoneStateDoneTransition));
	return (state == SkunkEventState::ZoneStateNoInfo || state == SkunkEventState::ZoneStateNoInvasion || state == SkunkEventState::ZoneStateDoneTransition) ? false : true;
}

SkunkEventState SkunkEvent::GetZoneState(Entity* self) {
	Game::logger->Log("SkunkEvent", "getting zone state");
	return self->GetVar<SkunkEventState>(u"ZoneState");
}

void SkunkEvent::SetZoneState(Entity* self, SkunkEventState state) {
	Game::logger->Log("SkunkEvent", "setting server zone state to %i", state);

	auto prevState = GetZoneState(self);

	self->SetVar<SkunkEventState>(u"ZoneState", state);


	if (prevState != state) {
		switch (state) {
		case SkunkEventState::ZoneStateNoInvasion:
			DoNoInvasionStateActions(self);
			break;
		case SkunkEventState::ZoneStateTransition:
			DoTransitionStateActions(self);
			break;
		case SkunkEventState::ZoneStateHighAlert:
			DoHighAlertStateActions(self);
			break;
		case SkunkEventState::ZoneStateMediumAlert:
			DoMediumAlertStateActions(self);
			break;
		case SkunkEventState::ZoneStateLowAlert:
			DoLowAlertStateActions(self);
			break;
		case SkunkEventState::ZoneStateDoneTransition:
			DoDoneTransitionActions(self);
			break;
		}
		GameMessages::SendNotifyClientZoneObject(self->GetObjectID(), u"zone_state_change", static_cast<int32_t>(state), 0, LWOOBJID_EMPTY, "", UNASSIGNED_SYSTEM_ADDRESS);
	}

}

void SkunkEvent::DoNoInvasionStateActions(Entity* self) {
	ResetTotalCleanPoints(self);

	SendStateToSpouts(self);

	SendStateToBubbleBlowers(self);

	self->AddTimer("startEventTimer", ZorilloConstants::peaceTimeDuration);
}

void SkunkEvent::DoTransitionStateActions(Entity* self) {
	SendStateToSpouts(self);

	SendStateToBubbleBlowers(self);

	ResetTotalCleanPoints(self);

	self->AddTimer("DoPanicNPCs", ZorilloConstants::earthquakeDuration);
	self->AddTimer("SkunksSpawning", ZorilloConstants::skunkSpawnTiming);
	self->AddTimer("StinkCloudsSpawning", ZorilloConstants::skunkSpawnTiming);
	self->AddTimer("HazmatVanTimer", ZorilloConstants::hazmatVanTiming);
	self->AddTimer("PoleSlideTimer", ZorilloConstants::poleSlideTiming);
	self->AddTimer("EndInvasionTransition", ZorilloConstants::invasionTransitionDuration);
}

void SkunkEvent::DoHighAlertStateActions(Entity* self) {
	self->AddTimer("MaxInvasionTimer", ZorilloConstants::maxInvasionDuration);
}

void SkunkEvent::DoMediumAlertStateActions(Entity* self) {
}

void SkunkEvent::DoLowAlertStateActions(Entity* self) {
}

void SkunkEvent::DoDoneTransitionActions(Entity* self) {
	self->CancelTimer("MaxInvationTimer");

	RewardPlayers(self);
	ResetTotalCleanPoints(self);

	float animTime = AnimateVan(self, u"end");

	if (animTime > 0.0f) self->AddTimer("HazmatVanEndDone", animTime);

	IdleNPCs(self);

	KillSkunks(self);

	KillStinkClouds(self);

	KillHazmatNPCs(self);

	self->AddTimer("EndDoneTransition", ZorilloConstants::doneTransitionDuration);
}

void SkunkEvent::SendStateToSpouts(Entity* self) {
	Game::logger->Log("SkunkEvent", "sending state to sprouts");
	auto spouts = EntityManager::Instance()->GetEntitiesByLOT(ZorilloConstants::spoutLot);
	for (auto* spout : spouts) spout->NotifyObject(self, "zone_state_change", static_cast<int32_t>(GetZoneState(self)));
}

void SkunkEvent::SendStateToBubbleBlowers(Entity* self) {
	Game::logger->Log("SkunkEvent", "sending state to bubble blowers");
	auto bubbleBlowers = EntityManager::Instance()->GetEntitiesByLOT(ZorilloConstants::bubbleBlowerLot);
	for (auto* bubbleBlower : bubbleBlowers) bubbleBlower->NotifyObject(self, "zone_state_change", static_cast<int32_t>(GetZoneState(self)));
}

uint32_t SkunkEvent::GetTotalCleanPoints(Entity* self) {
	return self->GetVar<uint32_t>(u"TotalCleanPoints");
}

bool SkunkEvent::IncrementTotalCleanPoints(Entity* self, uint32_t incrementedPoints) {

	if (!IsInvasionActive(self)) {
		Game::logger->Log("SkunkEvent", "no points added, invation not active");
		return false;
	}

	uint32_t points = GetTotalCleanPoints(self);
	points = points + incrementedPoints;
	self->SetVar<uint32_t>(u"TotalCleanPoints", points);

	Game::logger->Log("SkunkEvent", "clean points is now %i", points);

	if (GetZoneState(self) == SkunkEventState::ZoneStateHighAlert && points >= ZorilloConstants::cleaningPointsMedium) {
		SetZoneState(self, SkunkEventState::ZoneStateMediumAlert);
	} else if (GetZoneState(self) == SkunkEventState::ZoneStateMediumAlert && points >= ZorilloConstants::cleaningPointsLow) {
		SetZoneState(self, SkunkEventState::ZoneStateLowAlert);
	} else if (GetZoneState(self) == SkunkEventState::ZoneStateLowAlert && points >= ZorilloConstants::cleaningPointsTotal) {
		SetZoneState(self, SkunkEventState::ZoneStateDoneTransition);
		return true;
	}
	return false;
}

void SkunkEvent::ResetTotalCleanPoints(Entity* self) {
	self->SetVar<uint32_t>(u"TotalCleanPoints", 0);
}

void SkunkEvent::InitZoneVars(Entity* self) {
	auto stinkCloudPath = dZoneManager::Instance()->GetZone()->GetPath(ZorilloConstants::stinkCloudPath);
	if (!stinkCloudPath) return;
	for (auto path : stinkCloudPath->pathWaypoints) {
		Game::logger->Log("SkunkEvent", "path waypoints are %f %f %f", path.position.x, path.position.y, path.position.z);
	}
	self->SetVar<uint32_t>(u"NumStinkCloudSpawnPoints", stinkCloudPath->pathWaypoints.size());

	SetZoneState(self, SkunkEventState::ZoneStateNoInvasion);
	ResetTotalCleanPoints(self);
}

void SkunkEvent::OnStartup(Entity* self) {
	self->SetVar<SkunkEventState>(u"ZoneState", SkunkEventState::ZoneStateNoInfo);
	InitZoneVars(self);

	SpawnGarageVan(self);

	self->AddTimer("sporeTimer", 10.0f);
	self->AddTimer("startEventTimer", ZorilloConstants::peaceTimeDuration);
}

void SkunkEvent::OnChildLoaded(Entity* self, Entity* child) {
	if (IsValidSkunk(child->GetLOT())) {
		self->AddChild(child);

		auto skunkNum = child->GetVar<uint32_t>(u"SkunkNum");
		invasionSkunks.insert(std::make_pair(skunkNum, child));

		Game::logger->Log("SkunkEvent", "parent now has a child of skunk with id %llu", child->GetObjectID());
	} else if (child->GetLOT() == ZorilloConstants::spawnedHazmatNpc) {
		self->AddChild(child);

		auto num = child->GetVar<uint32_t>(u"HazmatNum");
		hazmatNpcs.insert(std::make_pair(num, child));

		Game::logger->Log("SkunkEvent", "parent now has a child of hazmat npc %llu", child->GetObjectID());

	} else if (child->GetLOT() == ZorilloConstants::invasionStinkCloudLot) {
		self->AddChild(child);
		auto num = child->GetVar<uint32_t>(u"StinkCloudNum");

		invationStinkClouds.insert(std::make_pair(num, child));

		Game::logger->Log("SkunkEvent", "child stink cloud %llu spawned at waypoint %i", child->GetObjectID(), num);
	} else if (child->GetLOT() == ZorilloConstants::hazmatRebuildVanLot) {
		self->AddChild(child);

		LWOOBJID spawnedVanId = self->GetVar<LWOOBJID>(u"HazmatVanID");
		if (spawnedVanId != LWOOBJID_EMPTY) {
			auto* vanEntity = EntityManager::Instance()->GetEntity(spawnedVanId);
			if (vanEntity) {
				Game::logger->Log("SkunkEvent", "smashing existing van");
				vanEntity->Smash();
			}
		}
		Game::logger->Log("SkunkEvent", "%llu %llu", spawnedVanId, child->GetObjectID());
		self->SetVar<LWOOBJID>(u"HazmatVanID", child->GetObjectID());

		self->AddTimer("SpawnHazmatNPCTimer", ZorilloConstants::hazmatNpcSpawnTimer);
	} else if (child->GetLOT() == ZorilloConstants::hazmatVanLot) {
		Game::logger->Log("SkunkEvent", "spawning a hazmat van");
		self->AddChild(child);

		LWOOBJID spawnedVanId = self->GetVar<LWOOBJID>(u"HazmatVanID");
		if (spawnedVanId != LWOOBJID_EMPTY) {
			auto* vanEntity = EntityManager::Instance()->GetEntity(spawnedVanId);
			if (vanEntity) {
				Game::logger->Log("SkunkEvent", "2 smashing van");
				vanEntity->Smash();
			}
		}

		self->SetVar<LWOOBJID>(u"HazmatVanID", child->GetObjectID());
	}
}

void SkunkEvent::OnObjectLoaded(Entity* self, Entity* loadedEntity) {
	if (IsValidNPC(loadedEntity->GetLOT())) {
		uint32_t nextActor = spawnedNpcs.size() + 1;
		spawnedNpcs.insert(std::make_pair(nextActor, loadedEntity));
	} else if (loadedEntity->GetLOT() == ZorilloConstants::spoutLot) {
		uint32_t nextSpout = spawnedSpouts.size() + 1;
		spawnedSpouts.insert(std::make_pair(nextSpout, loadedEntity));
	} else if (loadedEntity->GetLOT() == ZorilloConstants::bubbleBlowerLot) {
		uint32_t nextBlower = spawnedBubbleBlowers.size() + 1;
		spawnedBubbleBlowers.insert(std::make_pair(nextBlower, loadedEntity));
	} else if (loadedEntity->GetLOT() == ZorilloConstants::poleSlideNpc) {
		self->AddChild(loadedEntity);
		self->SetVar<LWOOBJID>(u"PoleSlideNPC", loadedEntity->GetObjectID());
	} else if (loadedEntity->GetLOT() == ZorilloConstants::balloonLot) {
		self->AddChild(loadedEntity);
		self->SetVar<LWOOBJID>(u"Balloon", loadedEntity->GetObjectID());
	} else if (loadedEntity->GetLOT() == ZorilloConstants::flowerLot) {
		uint32_t nextFlower = spawnedFlowers.size() + 1;
		spawnedFlowers.insert(std::make_pair(nextFlower, loadedEntity));
	}
}

void SkunkEvent::OnPlayerLoaded(Entity* self, Entity* player) {
	GameMessages::SendNotifyClientZoneObject(self->GetObjectID(), u"zone_state_change", static_cast<int32_t>(GetZoneState(self)), 0, LWOOBJID_EMPTY, "", player->GetSystemAddress());
	auto* balloon = EntityManager::Instance()->GetEntity(self->GetVar<LWOOBJID>(u"Balloon"));
	if (balloon) balloon->NotifyObject(self, "playerLoaded");

	for (auto flower : spawnedFlowers) {
		if (flower.second) flower.second->NotifyObject(self, "playerLoaded");
	}
}

void SkunkEvent::OnTimerDone(Entity* self, std::string timerName) {
	Game::logger->Log("SkunkEvent", "timer %s ended", timerName.c_str());

	if (timerName == "startEventTimer") {
		SetZoneState(self, SkunkEventState::ZoneStateTransition);
	} else if (timerName == "MaxInvasionTimer") {
		SetZoneState(self, SkunkEventState::ZoneStateDoneTransition);
	} else if (timerName == "DoPanicNPCs") {
		PanicNPCs(self);
	} else if (timerName == "SkunksSpawning") {
		SpawnSkunks(self);
	} else if (timerName == "StinkCloudsSpawning") {
		SpawnStinkClouds(self);
	} else if (timerName == "EndInvasionTransition") {
		SetZoneState(self, SkunkEventState::ZoneStateHighAlert);
	} else if (timerName == "EndDoneTransition") {
		SetZoneState(self, SkunkEventState::ZoneStateNoInvasion);
	} else if (timerName == "HazmatVanTimer") {
		auto animTime = AnimateVan(self, u"start");

		if (animTime > 0) self->AddTimer("HazmatVanStartDone", animTime);
		Game::logger->Log("SkunkEvent", "anim time was %f", animTime);
	} else if (timerName == "PoleSlideTimer") {
		auto* slider = EntityManager::Instance()->GetEntity(self->GetVar<LWOOBJID>(u"PoleSlideNPC"));

		if (slider) GameMessages::SendPlayAnimation(slider, u"slide");
	} else if (timerName == "HazmatVanStartDone") {
		SpawnRebuildVan(self);
	} else if (timerName == "HazmatVanEndDone") {
		SpawnGarageVan(self);
	} else if (timerName == "SpawnHazmatNPCTimer") {
		SpawnHazmatNPCs(self);
	} else if (timerName.find("SpawnSingleHazmatNPCTimer") == 0) {
		std::string subStr = timerName.substr(25);
		int32_t num = -1;
		GeneralUtils::TryParse(subStr, num);
		if (num == -1) {
			Game::logger->Log("SkunkEvent", "1 failed to convert string to num! (%s)", timerName.c_str());
			return;
		}
		SpawnSingleHazmatNPC(self, num);
	} else if (timerName.find("RespawnSkunk") == 0) {
		if (IsInvasionActive(self)) {
			std::string subStr = timerName.substr(12);
			int32_t num = -1;
			GeneralUtils::TryParse(subStr, num);
			if (num == -1) {
				Game::logger->Log("SkunkEvent", "2 failed to convert string to num! (%s)", timerName.c_str());
				return;
			}
			SpawnSingleSkunk(self, num, true);
		}
	} else if (timerName == "sporeTimer") {
		LoadSporeAnimals(self);
	}
}

void SkunkEvent::SpawnGarageVan(Entity* self) {
	auto path = dZoneManager::Instance()->GetZone()->GetPath(ZorilloConstants::hazmatRebuildVanSpawnPath);
	if (!path) return;
	PathWaypoint startingWaypoint;
	if (path->waypointCount > 0) startingWaypoint = path->pathWaypoints.at(0);
	else return;

	EntityInfo spawnedVanInfo{};
	spawnedVanInfo.lot = ZorilloConstants::hazmatVanLot;
	spawnedVanInfo.pos = startingWaypoint.position;
	spawnedVanInfo.rot = startingWaypoint.rotation;
	spawnedVanInfo.spawnerID = self->GetObjectID();

	auto* createdVanEntity = EntityManager::Instance()->CreateEntity(spawnedVanInfo, nullptr, self);
	Game::logger->Log("SkunkEvent", "spawned a van %i at %f %f %f", createdVanEntity->GetLOT(), startingWaypoint.position.x, startingWaypoint.position.y, startingWaypoint.position.z);
	if (createdVanEntity) EntityManager::Instance()->ConstructEntity(createdVanEntity);
}

void SkunkEvent::SpawnRebuildVan(Entity* self) {
	auto path = dZoneManager::Instance()->GetZone()->GetPath(ZorilloConstants::hazmatRebuildVanSpawnPath);
	if (!path) return;
	PathWaypoint startingWaypoint;
	if (path->waypointCount > 0) startingWaypoint = path->pathWaypoints.at(1);
	else return;

	EntityInfo spawnedVanInfo{};
	spawnedVanInfo.lot = ZorilloConstants::hazmatRebuildVanLot;
	spawnedVanInfo.pos = startingWaypoint.position;
	spawnedVanInfo.rot = startingWaypoint.rotation;
	spawnedVanInfo.spawnerID = self->GetObjectID();

	auto* createdVanEntity = EntityManager::Instance()->CreateEntity(spawnedVanInfo, nullptr, self);
	Game::logger->Log("SkunkEvent", "spawned a rebuild van %i at %f %f %f", createdVanEntity->GetLOT(), startingWaypoint.position.x, startingWaypoint.position.y, startingWaypoint.position.z);
	if (createdVanEntity) EntityManager::Instance()->ConstructEntity(createdVanEntity);
}

void SkunkEvent::SpawnSkunks(Entity* self) {
	auto skunksToSpawn = ZorilloConstants::numSkunks;
	while (skunksToSpawn > 0) {
		SpawnSingleSkunk(self, skunksToSpawn, false);
		skunksToSpawn--;
	}
}

void SkunkEvent::SpawnSingleSkunk(Entity* self, uint32_t pathNumber, bool respawn) {
	Game::logger->Log("SkunkEvent", "spawning a skunk at %s%i", ZorilloConstants::skunkPathPrefix.c_str(), pathNumber);
	std::string pathToFollow = ZorilloConstants::skunkPathPrefix + std::to_string(pathNumber);
	uint32_t pathStart = 1;

	if (respawn) {
		pathToFollow = pathToFollow + ZorilloConstants::skunkRoamPathSuffix;
		pathStart = GetRandomWaypoint(pathToFollow);
	}

	std::vector<LDFBaseData*> settings{};
	settings.push_back(new LDFData<uint32_t>(u"SkunkNum", pathNumber));
	settings.push_back(new LDFData<std::string>(u"attached_path", pathToFollow));
	settings.push_back(new LDFData<uint32_t>(u"attached_path_start", pathStart - 1));
	settings.push_back(new LDFData<bool>(u"IsImmune", !respawn));

	auto* path = dZoneManager::Instance()->GetZone()->GetPath(pathToFollow);
	PathWaypoint waypointToFollow;
	if (path) {
		Game::logger->Log("SkunkEvent", "using path num %i", pathStart - 1);
		for (auto p : path->pathWaypoints) Game::logger->Log("SkunkEvent", "%s path points are %f %f %f", pathToFollow.c_str(), p.position.x, p.position.y, p.position.z);
		waypointToFollow = path->pathWaypoints.at(pathStart - 1);
	} else {
		Game::logger->Log("SkunkEvent", "no path found %s", pathToFollow);
		return;
	}

	LOT randomTemplate = ZorilloConstants::invasionSkunkLot.at(GeneralUtils::GenerateRandomNumber<LOT>(static_cast<size_t>(1), ZorilloConstants::invasionSkunkLot.size()) - 1);
	EntityInfo spawnedSkunkInfo;
	spawnedSkunkInfo.lot = randomTemplate;
	spawnedSkunkInfo.pos = waypointToFollow.position;
	spawnedSkunkInfo.spawnerID = self->GetObjectID();
	spawnedSkunkInfo.settings = settings;

	auto* spawnedSkunk = EntityManager::Instance()->CreateEntity(spawnedSkunkInfo, nullptr, self);
	Game::logger->Log("SkunkEvent", "skunk %i spawned at %f %f %f", spawnedSkunk->GetLOT(), spawnedSkunkInfo.pos.x, spawnedSkunkInfo.pos.y, spawnedSkunkInfo.pos.z);
	if (spawnedSkunk) EntityManager::Instance()->ConstructEntity(spawnedSkunk);
}

uint32_t SkunkEvent::GetRandomWaypoint(std::string path) {
	auto* lookedUpPath = dZoneManager::Instance()->GetZone()->GetPath(path);
	return (lookedUpPath && lookedUpPath->waypointCount > 0) ? GeneralUtils::GenerateRandomNumber<uint32_t>(1, lookedUpPath->waypointCount) : 1;
}

void SkunkEvent::KillSkunks(Entity* self) {
	for (auto pair : invasionSkunks) {
		if (pair.second) {
			Game::logger->Log("SkunkEvent", "killing skunk");
			pair.second->Smash();
		} else {
			Game::logger->Log("SkunkEvent", "Null skunk found in array of skunks while killing skunks");
		}
	}
	invasionSkunks.clear();
}

bool SkunkEvent::IsWaypointValid(Entity* self, uint32_t waypoint) {

	if (waypoint <= 0) {
		return false;
	}

	for (auto pair : spawnedStinkCloudWaypoints) {
		if (pair.second == waypoint) return false;
	}
	return true;
}

void SkunkEvent::SpawnStinkClouds(Entity* self) {
	auto stinkCloudsLeft = ZorilloConstants::numStinkClouds;
	while (stinkCloudsLeft > 0) {
		SpawnSingleStinkCloud(self, stinkCloudsLeft);
		stinkCloudsLeft--;
	}
}

void SkunkEvent::SpawnSingleStinkCloud(Entity* self, uint32_t num) {
	uint32_t maxPoints = self->GetVar<uint32_t>(u"NumStinkCloudSpawnPoints");

	if (maxPoints == 0) return;

	constexpr uint32_t maxTries = 20;

	uint32_t randomWaypoint = GeneralUtils::GenerateRandomNumber<uint32_t>(1, maxPoints);

	uint32_t numTries = 0;
	while (!IsWaypointValid(self, randomWaypoint)) {
		randomWaypoint = GeneralUtils::GenerateRandomNumber<uint32_t>(1, maxPoints);
		numTries = numTries++;
		if (numTries >= maxTries) {
			Game::logger->Log("SkunkEvent", "too many tries, add more waypoints");
			return;
		}
	}
	spawnedStinkCloudWaypoints.insert(std::make_pair(num, randomWaypoint));
	std::vector<LDFBaseData*> settings{};
	settings.push_back(new LDFData<uint32_t>(u"StinkCloudNum", num));

	auto pathStr = ZorilloConstants::stinkCloudPath;
	Game::logger->Log("SkunkEvent", "getting waypoint %s", pathStr.c_str());
	auto* lookedUpPath = dZoneManager::Instance()->GetZone()->GetPath(pathStr);
	NiPoint3 spawnPosition{};
	Game::logger->Log("SkunkEvent", "found path %s %i, random point is %i", pathStr.c_str(), (lookedUpPath != nullptr), randomWaypoint);
	if (lookedUpPath && randomWaypoint <= lookedUpPath->waypointCount) {
		Game::logger->Log("SkunkEvent", "using way point %i of %i", randomWaypoint, lookedUpPath->waypointCount);
		spawnPosition = lookedUpPath->pathWaypoints.at(randomWaypoint - 1).position;
	}

	EntityInfo spawnedStinkCloudInfo{};
	spawnedStinkCloudInfo.pos = spawnPosition;
	spawnedStinkCloudInfo.lot = ZorilloConstants::invasionStinkCloudLot;
	spawnedStinkCloudInfo.spawnerID = self->GetObjectID();
	spawnedStinkCloudInfo.settings = settings;

	auto* stinkCloudEntity = EntityManager::Instance()->CreateEntity(spawnedStinkCloudInfo, nullptr, self);
	Game::logger->Log("SkunkEvent", "spawned stink cloud %i at %f %f %f", stinkCloudEntity->GetLOT(), spawnPosition.x, spawnPosition.y, spawnPosition.z);
	if (stinkCloudEntity) EntityManager::Instance()->ConstructEntity(stinkCloudEntity);
}

void SkunkEvent::KillStinkClouds(Entity* self) {
	for (auto pair : invationStinkClouds) {
		if (pair.second) {
			Game::logger->Log("SkunkEvent", "killing stink cloud");
			pair.second->Smash();
		}
	}
	invationStinkClouds.clear();
	spawnedStinkCloudWaypoints.clear();
}

void SkunkEvent::AddPlayerPoints(Entity* self, Entity* player, uint32_t points) {

	if (!IsInvasionActive(self)) {
		Game::logger->Log("SkunkEvent", "No event active, no points added");
		return;
	}

	if (points <= 0 || !player || player->GetIsDead()) return;

	player->SetVar<uint32_t>(u"invasionPlayerScore", player->GetVar<uint32_t>(u"invasionPlayerScore") + 1);
	Game::logger->Log("SkunkEvent", "player %llu now has %i points this event", player->GetObjectID(), player->GetVar<uint32_t>(u"invasionPlayerScore"));

	auto foundPlayer = std::find(invasionPlayers.begin(), invasionPlayers.end(), player);

	if (foundPlayer == invasionPlayers.end()) invasionPlayers.push_back(player);
}

void SkunkEvent::RewardPlayers(Entity* self) {
	for (auto player : invasionPlayers) {
		if (player && !player->GetIsDead()) {
			uint32_t rewardedCoins = player->GetVar<uint32_t>(u"invasionPlayerScore") * ZorilloConstants::rewardMultiplier;
			LootGenerator::Instance().DropLoot(player, player, -1, rewardedCoins, rewardedCoins);
			GameMessages::SendDropClientLoot(player, LWOOBJID_EMPTY, LOT_NULL, rewardedCoins, player->GetPosition());
			Game::logger->Log("SkunkEvent", "rewarding player %llu with a score of %i", player->GetObjectID(), rewardedCoins / ZorilloConstants::rewardMultiplier);
		}
	}
	invasionPlayers.clear();
}

void SkunkEvent::OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1, int32_t param2) {
	if (name == "skunk_cleaned") {
		if (!IsInvasionActive(self)) return;

		AddPlayerPoints(self, sender, ZorilloConstants::pointValueSkunk);
		if (!IncrementTotalCleanPoints(self, ZorilloConstants::pointValueSkunk)) {
			if (param1 > 0) {
				auto respawnTime = GeneralUtils::GenerateRandomNumber<uint32_t>(ZorilloConstants::skunkRespawnTimerMin, ZorilloConstants::skunkRespawnTimerMax);
				self->AddTimer("RespawnSkunk" + std::to_string(param1), respawnTime);
			}
		}
	} else if (name == "stink_cloud_cleaned_by_player" || name == "stink_cloud_cleaned_by_broombot") {
		if (!IsInvasionActive(self)) return;

		if (name == "stink_cloud_cleaned_by_player") AddPlayerPoints(self, sender, ZorilloConstants::pointValueStinkCloud);
		if (!IncrementTotalCleanPoints(self, ZorilloConstants::pointValueStinkCloud)) SpawnSingleStinkCloud(self, param1);
	} else if (name == "hazmat_cleaned") {
		if (!IsInvasionActive(self)) return;

		AddPlayerPoints(self, sender, ZorilloConstants::pointValueHazmat);
		IncrementTotalCleanPoints(self, ZorilloConstants::pointValueHazmat);
	} else if (name == "broombot_fixed") {
		if (!IsInvasionActive(self)) return;

		AddPlayerPoints(self, sender, ZorilloConstants::pointValueBroombot);
		IncrementTotalCleanPoints(self, ZorilloConstants::pointValueBroombot);
	}
}

void SkunkEvent::PanicNPCs(Entity* self) {
	for (auto pair : spawnedNpcs) {
		if (pair.second) pair.second->NotifyObject(self, "npc_panic");
	}
}

void SkunkEvent::IdleNPCs(Entity* self) {
	for (auto pair : spawnedNpcs) {
		if (pair.second) pair.second->NotifyObject(self, "npc_idle");
	}
}

bool SkunkEvent::IsValidNPC(LOT templateId) {
	if (ZorilloConstants::invasionPanicActors.empty()) return false;

	for (auto lot : ZorilloConstants::invasionPanicActors) {
		if (templateId == lot) return true;
	}
	return false;
}

bool SkunkEvent::IsValidSkunk(LOT templateId) {
	if (ZorilloConstants::invasionSkunkLot.empty()) return false;

	for (auto lot : ZorilloConstants::invasionSkunkLot) {
		if (templateId == lot) return true;
	}
	return false;
}

float SkunkEvent::AnimateVan(Entity* self, std::u16string name) {
	auto* hazmatVan = EntityManager::Instance()->GetEntity(self->GetVar<LWOOBJID>(u"HazmatVanID"));
	float animTime = 0.0f;
	Game::logger->Log("SkunkEvent", "hazmat van is null %i", hazmatVan == nullptr);
	if (!hazmatVan) return animTime;

	hazmatVan->CancelAllTimers();

	if (name == u"start") animTime = 6.73299980163574f;
	else if (name == u"end") animTime = 5.33300018310547f;
	else Game::logger->Log("SkunkEvent", "Unsupported lookup of name %s", name.c_str());

	GameMessages::SendPlayAnimation(hazmatVan, name);

	return animTime;
}

void SkunkEvent::SpawnHazmatNPCs(Entity* self) {
	auto hazmatNpcNum = ZorilloConstants::numHazmatNpcs;
	while (hazmatNpcNum > 0) {
		self->AddTimer("SpawnSingleHazmatNPCTimer" + std::to_string(hazmatNpcNum), ZorilloConstants::timeBetweenHazmatSpawns * hazmatNpcNum);
		hazmatNpcNum--;
	}
}

void SkunkEvent::SpawnSingleHazmatNPC(Entity* self, uint32_t num) {
	auto pathStr = ZorilloConstants::hazmatNpcPathPrefix + std::to_string(num);

	std::vector<LDFBaseData*> settings{};
	settings.push_back(new LDFData<uint32_t>(u"HazmatNum", num));
	settings.push_back(new LDFData<std::string>(u"attached_path", pathStr));
	settings.push_back(new LDFData<uint32_t>(u"attached_path_start", 0));

	auto* lookedUpPath = dZoneManager::Instance()->GetZone()->GetPath(pathStr);
	NiQuaternion rotation(1.0f, 0.0f, 0.0f, 0.0f);
	NiPoint3 firstWaypoint{};
	if (lookedUpPath) {
		firstWaypoint = lookedUpPath->pathWaypoints.front().position;
	}

	auto* vanEntity = EntityManager::Instance()->GetEntity(self->GetVar<LWOOBJID>(u"HazmatVanID"));
	if (vanEntity) {
		Game::logger->Log("SkunkEvent", "adjusting rotation");
		rotation.x = 0.0 - vanEntity->GetRotation().z;
		rotation.y = 0.0 - vanEntity->GetRotation().w;
		rotation.z = vanEntity->GetRotation().x;
		rotation.w = vanEntity->GetRotation().y;
	}

	EntityInfo spawnedHazmatVanInfo;
	spawnedHazmatVanInfo.pos = firstWaypoint;
	spawnedHazmatVanInfo.rot = rotation;
	spawnedHazmatVanInfo.lot = ZorilloConstants::spawnedHazmatNpc;
	spawnedHazmatVanInfo.settings = settings;
	spawnedHazmatVanInfo.spawnerID = self->GetObjectID();

	auto* spawnedHazmatVan = EntityManager::Instance()->CreateEntity(spawnedHazmatVanInfo, nullptr, self);
	Game::logger->Log("SkunkEvent", "spawned van %i at %f %f %f", spawnedHazmatVan->GetLOT(), firstWaypoint.x, firstWaypoint.y, firstWaypoint.z);
	if (spawnedHazmatVan) EntityManager::Instance()->ConstructEntity(spawnedHazmatVan);
}

void SkunkEvent::KillHazmatNPCs(Entity* self) {
	Game::logger->Log("SkunkEvent", "killing hazmat npcs");
	for (auto pair : hazmatNpcs) {
		if (pair.second) {
			Game::logger->Log("SkunkEvent", "killing hazmat npc %i", pair.second->GetObjectID());
			pair.second->Smash();
		}
	}
	hazmatNpcs.clear();
}

void SkunkEvent::OnRequestFollow(Entity* self, Entity* requestor) {

	auto* motherEntity = EntityManager::Instance()->GetEntity(self->GetVar<LWOOBJID>(u"MotherSkunkID"));

	if (motherEntity) {
		Game::logger->Log("SkunkEvent", "Mother not null");
		// Send GameMessage RequestFollow
	}
}

void SkunkEvent::OnFireEventServerSide(Entity* self, Entity* sender, std::string args, int32_t param1, int32_t param2, int32_t param3) {
	if (args == "toggleEvent") {
		self->CancelAllTimers();
		auto state = GetZoneState(self);

		if (state == SkunkEventState::ZoneStateDoneTransition) state = SkunkEventState::ZoneStateNoInvasion;
		else state = static_cast<SkunkEventState>(static_cast<int32_t>(state) + 1);

		SetZoneState(self, state);
	}
}

void SkunkEvent::CancelAllTimers(Entity* self) {
	self->CancelTimer("startEventTimer");
	self->CancelTimer("skunkSpawnTimer");
	self->CancelTimer("fountainTimer");
	self->CancelTimer("shutterTimer");
	self->CancelTimer("stopEventTimer");
	self->CancelTimer("HazmatTruckTimer");
	self->CancelTimer("HazmatNPCTimer");
	self->CancelTimer("sirenTimer");
}

void SkunkEvent::LoadSporeAnimals(Entity* self) {
	int32_t startingNum = 3;
	int32_t copyOfStartingNum = startingNum;
	while (copyOfStartingNum > 0) {
		auto* lookedUpPath = dZoneManager::Instance()->GetZone()->GetPath("spore1");
		NiPoint3 firstWaypoint{};
		if (lookedUpPath) {
			firstWaypoint = lookedUpPath->pathWaypoints.at(copyOfStartingNum - 1).position;
		}
		EntityInfo spawnedSpore1;
		spawnedSpore1.pos = firstWaypoint;
		spawnedSpore1.spawnerID = self->GetObjectID();
		spawnedSpore1.lot = 3712;
		spawnedSpore1.rot = NiQuaternion(0.0f, 0.0f, 0.0f, 0.0f);

		auto* spawnedSpore = EntityManager::Instance()->CreateEntity(spawnedSpore1, nullptr, self);
		Game::logger->Log("SkunkEvent", "spawned a spore 1 %i at %f %f %f", spawnedSpore->GetLOT(), firstWaypoint.x, firstWaypoint.y, firstWaypoint.z);
		if (spawnedSpore) EntityManager::Instance()->ConstructEntity(spawnedSpore);
		copyOfStartingNum--;
	}

	copyOfStartingNum = startingNum;
	while (copyOfStartingNum > 0) {
		auto* lookedUpPath = dZoneManager::Instance()->GetZone()->GetPath("spore2");
		NiPoint3 firstWaypoint{};
		if (lookedUpPath) {
			firstWaypoint = lookedUpPath->pathWaypoints.at(copyOfStartingNum - 1).position;
		}
		EntityInfo spawnedSpore2;
		spawnedSpore2.pos = firstWaypoint;
		spawnedSpore2.spawnerID = self->GetObjectID();
		spawnedSpore2.lot = 3713;
		spawnedSpore2.rot = NiQuaternion(0.0f, 0.0f, 0.0f, 0.0f);

		auto* spawnedSpore = EntityManager::Instance()->CreateEntity(spawnedSpore2, nullptr, self);
		Game::logger->Log("SkunkEvent", "spawned a spore 2 %i at %f %f %f", spawnedSpore->GetLOT(), firstWaypoint.x, firstWaypoint.y, firstWaypoint.z);
		if (spawnedSpore) EntityManager::Instance()->ConstructEntity(spawnedSpore);
		copyOfStartingNum--;
	}

	copyOfStartingNum = startingNum;
	while (copyOfStartingNum > 0) {
		auto* lookedUpPath = dZoneManager::Instance()->GetZone()->GetPath("spore3");
		NiPoint3 firstWaypoint{};
		if (lookedUpPath) {
			firstWaypoint = lookedUpPath->pathWaypoints.at(copyOfStartingNum - 1).position;
		}
		EntityInfo spawnedSpore3;
		spawnedSpore3.pos = firstWaypoint;
		spawnedSpore3.spawnerID = self->GetObjectID();
		spawnedSpore3.lot = 3714;
		spawnedSpore3.rot = NiQuaternion(0.0f, 0.0f, 0.0f, 0.0f);

		auto* spawnedSpore = EntityManager::Instance()->CreateEntity(spawnedSpore3, nullptr, self);
		Game::logger->Log("SkunkEvent", "spawned a spore 3 %i at %f %f %f", spawnedSpore->GetLOT(), firstWaypoint.x, firstWaypoint.y, firstWaypoint.z);
		if (spawnedSpore) EntityManager::Instance()->ConstructEntity(spawnedSpore);
		copyOfStartingNum--;
	}
}
