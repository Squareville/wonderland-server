#include "SkunkEvent.h"
#include "StringifiedEnum.h"
#include "dZoneManager.h"
#include "RenderComponent.h"
#include "QuickBuildComponent.h"
#include "Loot.h"

#include <ranges>

namespace {
	std::map<int32_t, int32_t> g_InvasionStinkCloudWaypoints{};
	std::map<LWOOBJID, int32_t> g_PlayerPoints{};
	std::vector<LWOOBJID> g_SpawnedNpcs{};
	std::vector<LWOOBJID> g_Spouts{};
	std::vector<LWOOBJID> g_BubbleBlowers{};
	std::vector<LWOOBJID> g_InvasionSkunks{};
	std::vector<LWOOBJID> g_HazmatNpcs{};
	std::vector<LWOOBJID> g_InvasionStinkClouds{};
};

SkunkEventZoneState SkunkEvent::GetZoneState(const Entity* const self) const {
	return static_cast<SkunkEventZoneState>(self->GetVar<int32_t>(u"ZoneState"));
}

void SkunkEvent::SendStateToEntities(Entity* const self, const std::vector<LWOOBJID>& entities) const {
	for (const auto id : entities) {
		auto* const entity = Game::entityManager->GetEntity(id);
		if (entity) entity->NotifyObject(self, "zone_state_change", GeneralUtils::ToUnderlying(GetZoneState(self)));
	}
}

void SkunkEvent::DoNoInvasionStateActions(Entity* const self) const {
	ResetTotalCleanPoints(self);
	SendStateToEntities(self, g_Spouts);
	SendStateToEntities(self, g_BubbleBlowers);
	self->AddTimer("startEventTimer", PEACE_TIME_DURATION);
}

void SkunkEvent::DoTransitionStateActions(Entity* const self) const {
	SendStateToEntities(self, g_Spouts);
	SendStateToEntities(self, g_BubbleBlowers);
	ResetTotalCleanPoints(self);
	self->AddTimer("DoPanicNPCs", EARTHQUAKE_DURATION);
	self->AddTimer("SkunksSpawning", SKUNK_SPAWN_TIMING);
	self->AddTimer("StinkCloudsSpawning", SKUNK_SPAWN_TIMING);
	self->AddTimer("HazmatVanTimer", HAZMAT_VAN_TIMING);
	self->AddTimer("PoleSlideTimer", POLE_SLIDE_TIMING);
	self->AddTimer("EndInvasionTransition", INVASION_TRANSITION_DURATION);
}

void SkunkEvent::DoHighAlertStateActions(Entity* const self) const {
	self->AddTimer("MaxInvasionTimer", MAX_INVASION_DURATION);
}

void SkunkEvent::KillEntities(Entity* const self, const std::vector<LWOOBJID>& entities) const {
	for (const auto id : entities) {
		auto* const entity = Game::entityManager->GetEntity(id);
		if (entity) entity->Smash(id, eKillType::SILENT);
	}
}

void SkunkEvent::KillSkunks(Entity* const self) const {
	KillEntities(self, g_InvasionSkunks);
	g_InvasionSkunks.clear();
}

void SkunkEvent::KillStinkClouds(Entity* const self) const {
	KillEntities(self, g_InvasionStinkClouds);
	g_InvasionStinkClouds.clear();
	g_InvasionStinkCloudWaypoints.clear();
}

void SkunkEvent::KillHazmatNpcs(Entity* const self) const {
	KillEntities(self, g_HazmatNpcs);
	g_HazmatNpcs.clear();
}

float SkunkEvent::AnimateVan(Entity* const self, const std::string& animName) const {
	auto* const van = GetEntityByName(self, u"HazmatVanID");
	float animTime = 0.0f;
	if (van) {
		van->CancelAllTimers();
		animTime = RenderComponent::PlayAnimation(van, animName);
	}

	return animTime;
}

void SkunkEvent::RewardPlayers(Entity* const self) const {
	for (const auto& [playerId, points] : g_PlayerPoints) {
		auto* const player = Game::entityManager->GetEntity(playerId);
		if (player) {
			const auto coins = points * REWARD_MULTIPLIER;
			Loot::DropLoot(player, player, -1, coins, coins);
		}
	}
}

void SkunkEvent::DoDoneTransitionActions(Entity* const self) const {
	self->CancelTimer("MaxInvasionTimer");
	RewardPlayers(self);
	ResetTotalCleanPoints(self);
	float animTime = AnimateVan(self, "end");
	if (animTime > 0.0f) {
		self->AddTimer("HazmatVanEndDone", animTime);
	}
	NotifyNpcs(self, "npc_idle");
	KillSkunks(self);
	KillStinkClouds(self);
	KillHazmatNpcs(self);
	self->AddTimer("EndDoneTransition", DONE_TRANSITION_DURATION);
}

void SkunkEvent::SetZoneState(Entity* const self, const SkunkEventZoneState state) const {
	// LOG("New state is %s", StringifiedEnum::ToString(state).data());
	const auto oldState = GetZoneState(self);
	self->SetVar<int32_t>(u"ZoneState", GeneralUtils::ToUnderlying(state));
	if (oldState != state) {
		using enum SkunkEventZoneState;
		if (state == NO_INVASION) {
			DoNoInvasionStateActions(self);
		} else if (state == TRANSITION) {
			DoTransitionStateActions(self);
		} else if (state == HIGH_ALERT) {
			DoHighAlertStateActions(self);
		} else if (state == DONE_TRANSITION) {
			DoDoneTransitionActions(self);
		}
		// LOG("Sending notify client zone object");
		GameMessages::SendNotifyClientZoneObject(
			self->GetObjectID(), u"zone_state_change", GeneralUtils::ToUnderlying(state), 0, LWOOBJID_EMPTY, "", UNASSIGNED_SYSTEM_ADDRESS
		);
	}
}

bool SkunkEvent::InvasionActive(const Entity* const self) const {
	using enum SkunkEventZoneState;
	const auto zoneState = GetZoneState(self);
	// LOG("Zone state is %s", StringifiedEnum::ToString(zoneState).data());
	return zoneState == TRANSITION || zoneState == HIGH_ALERT || zoneState == MEDIUM_ALERT || zoneState == LOW_ALERT;
}

bool SkunkEvent::IncrementTotalCleanPoints(Entity* const self, const int32_t points) const {
	using enum SkunkEventZoneState;
	if (!InvasionActive(self)) return false;

	auto totalCleanPoints = self->GetVar<int32_t>(u"TotalCleanPoints");
	// LOG("Points are %i", points);
	totalCleanPoints += points;
	// LOG("Points are now %i", totalCleanPoints);
	self->SetVar<int32_t>(u"TotalCleanPoints", totalCleanPoints);

	const auto state = GetZoneState(self);
	if (state == HIGH_ALERT && totalCleanPoints >= CLEANING_POINTS_MEDIUM) {
		SetZoneState(self, MEDIUM_ALERT);
	} else if (state == MEDIUM_ALERT && totalCleanPoints >= CLEANING_POINTS_LOW) {
		SetZoneState(self, LOW_ALERT);
	} else if (state == LOW_ALERT && totalCleanPoints >= CLEANING_POINTS_TOTAL) {
		SetZoneState(self, DONE_TRANSITION);
		return true;
	}

	return false;
}

void SkunkEvent::SpawnSingleStinkCloud(Entity* const self, const int32_t number) const {
	const auto maxPoints = self->GetVar<int32_t>(u"NumStinkCloudSpawnPoints");
	// LOG("Max points are %i", maxPoints);
	if (maxPoints == 0) return;
	int32_t maxTries = 20;
	int32_t waypoint{};

	do {
		waypoint = GeneralUtils::GenerateRandomNumber<int32_t>(1, maxPoints);
		if (maxTries-- <= 0) {
			LOG("Max tries reached");
			return;
		}
	} while (!IsValidWaypoint(self, waypoint));

	// LOG("Chose waypoint %i", waypoint);
	g_InvasionStinkCloudWaypoints[number] = waypoint;

	const auto* const path = Game::zoneManager->GetZone()->GetPath(STINK_CLOUD_PATH);
	// LOG("Valid waypoint %p", path);
	if (!path || waypoint > path->pathWaypoints.size()) return;
	auto [x, y, z] = path->pathWaypoints[waypoint - 1].position;
	// LOG("Position is %f %f %f", x, y, z);
	EntityInfo info{};
	info.lot = INVASION_STINK_CLOUD_LOT;
	info.settings = { new LDFData<int32_t>(u"StinkCloudNum", number) };
	info.pos = path->pathWaypoints[waypoint - 1].position;
	info.spawnerID = self->GetObjectID();

	auto* const newStinkCloud = Game::entityManager->CreateEntity(info, nullptr, self);
	Game::entityManager->ConstructEntity(newStinkCloud);
}

bool SkunkEvent::IsValidWaypoint(const Entity* const self, const int32_t waypoint) const {
	// LOG("Checking waypoint %i", waypoint);
	if (waypoint < 1) return false;
	// LOG("contains %i", g_InvasionStinkCloudWaypoints.contains(waypoint));

	for (const auto value : g_InvasionStinkCloudWaypoints | std::views::values) {
		if (value == waypoint) return false;
	}

	return true;
}

void SkunkEvent::AddPlayerPoints(const Entity* const self, const LWOOBJID player, const int32_t points) const {
	// LOG("Vars are %i %i %i", InvasionActive(self), player, points);
	if (!InvasionActive(self) || player == LWOOBJID_EMPTY || points <= 0) return;
	// LOG("Player points are %i", g_PlayerPoints[player]);
	g_PlayerPoints[player] += points;
	// LOG("Player points are now %i", g_PlayerPoints[player]);
}

void SkunkEvent::OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1, int32_t param2) {
	if (!InvasionActive(self)) return;

	// LOG("SkunkEvent::OnNotifyObject: %s param1 %i", name.c_str(), param1);
	if (name.starts_with(SKUNK_PATH_PREFIX)) {
		AddPlayerPoints(self, sender->GetObjectID(), POINT_VALUE_SKUNK);
		if (!IncrementTotalCleanPoints(self, POINT_VALUE_SKUNK)) {
			// LOG("Spawning skunk");
			self->AddTimer(name, GeneralUtils::GenerateRandomNumber<float>(SKUNK_RESPAWN_TIMER_MIN, SKUNK_RESPAWN_TIMER_MAX));
		}
	} else if (name == "stink_cloud_cleaned_by_broombot") {
		if (!IncrementTotalCleanPoints(self, POINT_VALUE_STINK_CLOUD)) {
			SpawnSingleStinkCloud(self, param1);
		}
	} else if (name == "stink_cloud_cleaned_by_player") {
		AddPlayerPoints(self, sender->GetObjectID(), POINT_VALUE_STINK_CLOUD);
		if (!IncrementTotalCleanPoints(self, POINT_VALUE_STINK_CLOUD)) {
			SpawnSingleStinkCloud(self, param1);
		}
	} else if (name == "hazmat_cleaned") {
		AddPlayerPoints(self, sender->GetObjectID(), POINT_VALUE_HAZMAT);
		IncrementTotalCleanPoints(self, POINT_VALUE_HAZMAT);
	} else if (name == "broombot_fixed") {
		AddPlayerPoints(self, sender->GetObjectID(), POINT_VALUE_BROOMBOT);
		IncrementTotalCleanPoints(self, POINT_VALUE_BROOMBOT);
	}
}

void SkunkEvent::InitZoneVars(Entity* const self) const {
	// LOG("SkunkEvent::InitZoneVars");
	SetZoneState(self, SkunkEventZoneState::NO_INVASION);
	const auto* const path = Game::zoneManager->GetZone()->GetPath(STINK_CLOUD_PATH);

	if (path) {
		self->SetVar<int32_t>(u"NumStinkCloudSpawnPoints", path->pathWaypoints.size());
	}

	ResetTotalCleanPoints(self);
}

void SkunkEvent::SpawnGarageVan(Entity* const self) const {
	const auto* path = Game::zoneManager->GetZone()->GetPath(HAZMAT_REBUILD_VAN_SPAWN_PATH);
	if (!path || path->pathWaypoints.empty()) return;

	EntityInfo info{};
	info.pos = path->pathWaypoints[0].position;
	info.rot = path->pathWaypoints[0].rotation;
	info.lot = HAZMAT_VAN_LOT;
	info.spawnerID = self->GetObjectID();
	Game::entityManager->ConstructEntity(Game::entityManager->CreateEntity(info, nullptr, self));
}

void SkunkEvent::OnStartup(Entity* self) {
	InitZoneVars(self);
	SpawnGarageVan(self);
	self->AddTimer("startEventTimer", PEACE_TIME_DURATION);
}

void SkunkEvent::OnPlayerLoaded(Entity* self, Entity* player) {
	GameMessages::SendNotifyClientObject(self->GetObjectID(), u"", GeneralUtils::ToUnderlying(GetZoneState(self)), 0, LWOOBJID_EMPTY, "", player->GetSystemAddress());
}

void SkunkEvent::ResetTotalCleanPoints(Entity* const self) const {
	self->SetVar<int32_t>(u"TotalCleanPoints", 0);
}

void SkunkEvent::OnFireEventServerSide(Entity* self, Entity* sender, std::string args, int32_t param1, int32_t param2, int32_t param3) {
	// only for debug, never supposed to be called by the client
	if (args == "toggleEvent") {
		self->CancelAllTimers();
		auto state = GetZoneState(self);
		if (state == SkunkEventZoneState::DONE_TRANSITION) {
			state = SkunkEventZoneState::NO_INVASION; // loops around
		} else {
			state = static_cast<SkunkEventZoneState>(GeneralUtils::ToUnderlying(state) + 1);
		}
		SetZoneState(self, state);
	}
}

void SkunkEvent::NotifyNpcs(Entity* const self, const std::string& name) const {
	for (const auto id : g_SpawnedNpcs) {
		auto* const npc = Game::entityManager->GetEntity(id);
		if (!npc) continue;

		npc->NotifyObject(self, name);
	}
}

void SkunkEvent::SpawnRebuildVan(Entity* const self) const {
	const auto* path = Game::zoneManager->GetZone()->GetPath(HAZMAT_REBUILD_VAN_SPAWN_PATH);
	if (!path || path->pathWaypoints.size() < 2) return;

	EntityInfo info{};
	info.pos = path->pathWaypoints[1].position;
	info.rot = path->pathWaypoints[1].rotation;
	info.lot = HAZMAT_REBUILD_VAN_LOT;
	info.spawnerID = self->GetObjectID();
	Game::entityManager->ConstructEntity(Game::entityManager->CreateEntity(info, nullptr, self));
}

void SkunkEvent::SpawnStinkClouds(Entity* const self) const {
	for (int32_t i = 0; i < NUM_STINK_CLOUDS; ++i) {
		SpawnSingleStinkCloud(self, i);
	}
}

void SkunkEvent::SpawnHazmatNpcs(Entity* const self) const {
	for (int32_t i = 0; i < NUM_HAZMAT_NPCS; i++) {
		self->AddTimer(HAZMAT_NPC_PATH_PREFIX + std::to_string(i), TIME_BETWEEN_HAZMAT_SPAWNS * i);
	}
}

void SkunkEvent::SpawnSingleHazmatNpc(Entity* const self, const std::string& pathStr) const {
	const auto* const path = Game::zoneManager->GetZone()->GetPath(pathStr);
	if (!path || path->pathWaypoints.empty()) return;

	EntityInfo info{};
	info.pos = path->pathWaypoints[0].position;
	info.rot = NiQuaternionConstant::IDENTITY;
	auto* const van = GetEntityByName(self, u"HazmatVanID");
	if (van) {
		const auto& vanRotation = van->GetRotation();
		info.rot.x = vanRotation.x;
		info.rot.y = vanRotation.y;
		info.rot.z = 0.0 - vanRotation.z;
		info.rot.w = 0.0 - vanRotation.w;
	}
	info.settings = {
		new LDFData<std::string>(u"attached_path", pathStr),
		new LDFData<int32_t>(u"attached_path_start", 0),
	};
	info.lot = SPAWNED_HAZMAT_NPC;
	info.spawnerID = self->GetObjectID();
	Game::entityManager->ConstructEntity(Game::entityManager->CreateEntity(info, nullptr, self));
}

void SkunkEvent::SpawnSkunks(Entity* const self) const {
	for (int32_t i = 0; i < NUM_SKUNKS; i++) {
		SpawnSingleSkunk(self, i, false);
	}
}

void SkunkEvent::SpawnSingleSkunk(Entity* const self, const int32_t num, const bool respawn, const std::string& respawnPath) const {
	// LOG("respawn path %s num %i", respawnPath.c_str(), num);
	auto pathStr = SKUNK_PATH_PREFIX + std::to_string(num);
	auto pathStart = 1;
	auto* path = Game::zoneManager->GetZone()->GetPath(pathStr);

	if (respawn) {
		pathStr = respawnPath + SKUNK_ROAM_PATH_SUFFIX;
		path = Game::zoneManager->GetZone()->GetPath(pathStr);
		if (!path || path->pathWaypoints.empty()) return;
		// LOG("size is %i", path->pathWaypoints.size());
		pathStart = GeneralUtils::GenerateRandomNumber<int32_t>(0, path->pathWaypoints.size());
	}
	if (!path || path->pathWaypoints.empty()) return;

	EntityInfo info{};
	info.pos = path->pathWaypoints[pathStart - 1].position;
	info.spawnerID = self->GetObjectID();
	info.settings = {
		new LDFData<std::string>(u"attached_path", pathStr),
		new LDFData<std::string>(u"respawn_path", respawnPath.empty() ? pathStr : respawnPath),
		new LDFData<int32_t>(u"attached_path_start", pathStart),
		new LDFData<bool>(u"isImmune", !respawn),
	};
	info.lot = *(INVASION_SKUNK_LOT.begin() + GeneralUtils::GenerateRandomNumber<int32_t>(0, INVASION_SKUNK_LOT.size() - 1));

	Game::entityManager->ConstructEntity(Game::entityManager->CreateEntity(info, nullptr, self));
}

void SkunkEvent::OnTimerDone(Entity* self, std::string name) {
	// LOG("SkunkEvent::OnTimerDone: %s", name.c_str());
	if (name == "startEventTimer") {
		SetZoneState(self, SkunkEventZoneState::TRANSITION);
	} else if (name == "MaxInvasionTimer") {
		SetZoneState(self, SkunkEventZoneState::DONE_TRANSITION);
	} else if (name == "DoPanicNPCs") {
		NotifyNpcs(self, "npc_panic");
	} else if (name == "SkunksSpawning") {
		SpawnSkunks(self);
	} else if (name == "StinkCloudsSpawning") {
		SpawnStinkClouds(self);
	} else if (name == "EndInvasionTransition") {
		SetZoneState(self, SkunkEventZoneState::HIGH_ALERT);
	} else if (name == "EndDoneTransition") {
		SetZoneState(self, SkunkEventZoneState::NO_INVASION);
	} else if (name == "HazmatVanTimer") {
		const auto animTime = AnimateVan(self, "start");
		if (animTime > 0.0f) {
			self->AddTimer("HazmatVanStartDone", animTime);
		}
	} else if (name == "PoleSlideTimer") {
		auto* const poleSlide = GetEntityByName(self, u"PoleSlideNPC");
		if (poleSlide) {
			RenderComponent::PlayAnimation(poleSlide, "slide");
		}
	} else if (name == "HazmatVanStartDone") {
		SpawnRebuildVan(self);
	} else if (name == "HazmatVanEndDone") {
		SpawnGarageVan(self);
	} else if (name == "SpawnHazmatNPCTimer") {
		SpawnHazmatNpcs(self);
	} else if (name.starts_with(HAZMAT_NPC_PATH_PREFIX)) {
		SpawnSingleHazmatNpc(self, name);
	} else if (name.starts_with(SKUNK_PATH_PREFIX)) {
		if (InvasionActive(self)) SpawnSingleSkunk(self, -1, true, name);
	}
}

bool SkunkEvent::IsValidNpc(const Entity* const self, const LOT lot) const {
	return std::ranges::find(INVASION_PANIC_ACTORS, lot) != INVASION_PANIC_ACTORS.end();
}

bool SkunkEvent::IsValidSkunk(const Entity* const self, const LOT lot) const {
	return std::ranges::find(INVASION_SKUNK_LOT, lot) != INVASION_SKUNK_LOT.end();
}

void SkunkEvent::OnObjectLoaded(Entity* self, LWOOBJID objId, LOT lot) {
	// LOG("Object loaded %llu:%i", objId, lot);
	if (IsValidNpc(self, lot)) {
		g_SpawnedNpcs.push_back(objId);
	} else if (lot == SPOUT_LOT) {
		g_Spouts.push_back(objId);
	} else if (lot == BUBBLE_BLOWER_LOT) {
		g_BubbleBlowers.push_back(objId);
	} else if (lot == POLE_SLIDE_NPC) {
		StoreParent(self, objId);
		StoreEntityByName(self, u"PoleSlideNPC", objId);
	} else if (lot == BALLOON_LOT) {
		StoreParent(self, objId);
		StoreEntityByName(self, u"Balloon", objId);
	}

	// print out all the objects
	// LOG("Npcs:");
	// for (const auto value : g_SpawnedNpcs) {
	// 	LOG("	%llu", value);
	// }
	// LOG("Spouts:");
	// for (const auto value : g_Spouts) {
	// 	LOG("	%llu", value);
	// }
	// LOG("BubbleBlowers:");
	// for (const auto value : g_BubbleBlowers) {
	// 	LOG("	%llu", value);
	// }
}

void SkunkEvent::OnChildLoaded(Entity* self, const LWOOBJID objectId, const LOT lot) {
	// LOG("Child loaded %llu:%i", objectId, lot);
	if (IsValidSkunk(self, lot)) {
		StoreParent(self, objectId);
		g_InvasionSkunks.push_back(objectId);
	} else if (lot == SPAWNED_HAZMAT_NPC) {
		StoreParent(self, objectId);
		g_HazmatNpcs.push_back(objectId);
	} else if (lot == INVASION_STINK_CLOUD_LOT) {
		StoreParent(self, objectId);
		g_InvasionStinkClouds.push_back(objectId);
	} else if (lot == HAZMAT_REBUILD_VAN_LOT) {
		StoreParent(self, objectId);
		auto* const van = GetEntityByName(self, u"HazmatVanID");
		if (van) {
			van->Smash(LWOOBJID_EMPTY, eKillType::SILENT);
		}
		StoreEntityByName(self, u"HazmatVanID", objectId);
		auto* const quickbuildComponent = self->GetComponent<QuickBuildComponent>();
		if (quickbuildComponent) {
			quickbuildComponent->ResetQuickBuild(false);
		}
		self->AddTimer("SpawnHazmatNPCTimer", HAZMAT_NPC_SPAWN_TIMER);
	} else if (lot == HAZMAT_VAN_LOT) {
		StoreParent(self, objectId);
		auto* const van = GetEntityByName(self, u"HazmatVanID");
		if (van) {
			van->Smash(LWOOBJID_EMPTY, eKillType::SILENT);
		}
		StoreEntityByName(self, u"HazmatVanID", objectId);
	}

	// print out all the objects
	// LOG("Invasion Skunks:");
	// for (const auto value : g_InvasionSkunks) {
	// 	LOG("	%llu", value);
	// }
	// LOG("Hazmat Npcs:");
	// for (const auto value : g_HazmatNpcs) {
	// 	LOG("	%llu", value);
	// }
	// LOG("Invasion Stink Clouds:");
	// for (const auto value : g_InvasionStinkClouds) {
	// 	LOG("	%llu", value);
	// }
	// auto* const van = GetEntityByName(self, u"HazmatVanID");
	// if (van) {
	// 	LOG("Hazmat Van:");
	// 	LOG("	%llu", van->GetObjectID());
	// }
}
