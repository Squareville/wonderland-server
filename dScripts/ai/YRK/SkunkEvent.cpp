#include "SkunkEvent.h"
#include "StringifiedEnum.h"
#include "dZoneManager.h"

#include <ranges>

namespace {
	std::map<int32_t, int32_t> g_InvasionStinkCloudWaypoints{};
	std::map<LWOOBJID, int32_t> g_PlayerPoints{};
	std::map<int32_t, LWOOBJID> g_SpawnedNpcs{};
	std::map<int32_t, LWOOBJID> g_Spouts{};
	std::map<int32_t, LWOOBJID> g_BubbleBlowers{};
};

SkunkEventZoneState SkunkEvent::GetZoneState(const Entity* const self) const {
	return static_cast<SkunkEventZoneState>(self->GetVar<int32_t>(u"ZoneState"));
}

void SkunkEvent::SetZoneState(Entity* const self, const SkunkEventZoneState state) const {
	LOG("New state is %s", StringifiedEnum::ToString(state).data());
	const auto oldState = GetZoneState(self);
	self->SetVar<int32_t>(u"ZoneState", GeneralUtils::ToUnderlying(state));
	if (oldState != state) {
		LOG("Sending notify client zone object");
		GameMessages::SendNotifyClientZoneObject(
			self->GetObjectID(), u"zone_state_change", GeneralUtils::ToUnderlying(state), 0, LWOOBJID_EMPTY, "", UNASSIGNED_SYSTEM_ADDRESS
		);
	}
}

bool SkunkEvent::InvasionActive(const Entity* const self) const {
	using enum SkunkEventZoneState;
	const auto zoneState = GetZoneState(self);
	LOG("Zone state is %s", StringifiedEnum::ToString(zoneState).data());
	return zoneState == TRANSITION || zoneState == HIGH_ALERT || zoneState == MEDIUM_ALERT || zoneState == LOW_ALERT;
}

bool SkunkEvent::IncrementTotalCleanPoints(Entity* const self, const int32_t points) const {
	using enum SkunkEventZoneState;
	if (!InvasionActive(self)) return false;

	auto totalCleanPoints = self->GetVar<int32_t>(u"TotalCleanPoints");
	LOG("Points are %i", points);
	totalCleanPoints += points;
	LOG("Points are now %i", totalCleanPoints);
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
	LOG("Max points are %i", maxPoints);
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

	LOG("Chose waypoint %i", waypoint);
	g_InvasionStinkCloudWaypoints[number] = waypoint;

	const auto* const path = Game::zoneManager->GetZone()->GetPath(STINK_CLOUD_PATH);
	LOG("Valid waypoint %p", path);
	if (!path || waypoint > path->pathWaypoints.size()) return;
	auto [x, y, z] = path->pathWaypoints[waypoint - 1].position;
	LOG("Position is %f %f %f", x, y, z);
	EntityInfo info{};
	info.lot = INVASION_STINK_CLOUD_LOT;
	info.settings = { new LDFData<int32_t>(u"StinkCloudNum", number) };
	info.pos = path->pathWaypoints[waypoint - 1].position;
	info.spawnerID = self->GetObjectID();

	auto* const newStinkCloud = Game::entityManager->CreateEntity(info, nullptr, self);
	Game::entityManager->ConstructEntity(newStinkCloud);
}

bool SkunkEvent::IsValidWaypoint(const Entity* const self, const int32_t waypoint) const {
	LOG("Checking waypoint %i", waypoint);
	if (waypoint < 1) return false;
	LOG("contains %i", g_InvasionStinkCloudWaypoints.contains(waypoint));

	for (const auto value : g_InvasionStinkCloudWaypoints | std::views::values) {
		if (value == waypoint) return false;
	}

	return true;
}

void SkunkEvent::AddPlayerPoints(const Entity* const self, const LWOOBJID player, const int32_t points) const {
	LOG("Vars are %i %i %i", InvasionActive(self), player, points);
	if (!InvasionActive(self) || player == LWOOBJID_EMPTY || points <= 0) return;
	LOG("Player points are %i", g_PlayerPoints[player]);
	g_PlayerPoints[player] += points;
	LOG("Player points are now %i", g_PlayerPoints[player]);
}

void SkunkEvent::OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1, int32_t param2) {
	if (!InvasionActive(self)) return;

	LOG("SkunkEvent::OnNotifyObject: %s param1 %i", name.c_str(), param1);
	if (name == "skunk_cleaned") {
		// nothing triggers this logic so im not implementing it
	} else if (name == "stink_cloud_cleaned_by_broombot") {
		if (IncrementTotalCleanPoints(self, POINT_VALUE_STINK_CLOUD)) {
			SpawnSingleStinkCloud(self, param1);
		}
	} else if (name == "stink_cloud_cleaned_by_player") {
		AddPlayerPoints(self, sender->GetObjectID(), POINT_VALUE_STINK_CLOUD);
		if (IncrementTotalCleanPoints(self, POINT_VALUE_STINK_CLOUD)) {
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
	LOG("SkunkEvent::InitZoneVars");
	SetZoneState(self, SkunkEventZoneState::NO_INVASION);
	const auto* const path = Game::zoneManager->GetZone()->GetPath(STINK_CLOUD_PATH);

	if (path) {
		self->SetVar<int32_t>(u"NumStinkCloudSpawnPoints", path->pathWaypoints.size());
	}

	ResetTotalStinkPoints(self);
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

void SkunkEvent::ResetTotalStinkPoints(Entity* const self) const {
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

void SkunkEvent::PanicNpcs(const Entity* const self) const {

}

void SkunkEvent::OnTimerDone(Entity* self, std::string name) {
	if (name == "startEventTimer") {
		//SetZoneState(self, SkunkEventZoneState::TRANSITION);
	} else if (name == "MaxInvasionTimer") {
		//SetZoneState(self, SkunkEventZoneState::DONE_TRANSITION);
	} else if (name == "DoPanicNPCs") {
		//PanicNpcs(self);
	} else if (name == "SkunksSpawning") {

	} else if (name == "StinkCloudsSpawning") {

	} else if (name == "EndInvasionTransition") {

	} else if (name == "EndDoneTransition") {

	} else if (name == "HazmatVanTimer") {

	} else if (name == "PoleSlideTimer") {

	} else if (name == "HazmatVanStartDone") {

	} else if (name == "HazmatVanEndDone") {

	} else if (name == "SpawnHazmatNPCTimer") {

	} else if (name == "SpawnSingleHazmatNPCTimer") {

	} else if (name == "RespawnSkunk") {

	}
}

bool SkunkEvent::IsValidNpc(const Entity* const self, const LOT lot) const {
	return std::ranges::find(INVASION_PANIC_ACTORS, lot) != INVASION_PANIC_ACTORS.end();
}

void SkunkEvent::StoreParent(const Entity* const self, const LWOOBJID other) const {
	const auto selfIdStr = "|" + std::to_string(self->GetObjectID());
	auto* const otherObj = Game::entityManager->GetEntity(other);
	if (otherObj) {
		otherObj->SetVar<std::string>(u"My_Parent_ID", selfIdStr);
		LOG("Stored parent %s to %llu", selfIdStr.c_str(), other);
	}
	else LOG("Failed to store parent %s", selfIdStr.c_str());
}

void SkunkEvent::StoreObjectByName(Entity* const self, const std::u16string& varName, const LWOOBJID other) const {
	const auto otherIdStr = "|" + std::to_string(other);
	LOG("Storing object %s to %llu", otherIdStr.c_str(), self->GetObjectID());
	self->SetVar<std::string>(varName, otherIdStr);
}

void SkunkEvent::OnObjectLoaded(Entity* self, LWOOBJID objId, LOT lot) {
	LOG("Object loaded %llu:%i", objId, lot);
	if (IsValidNpc(self, lot)) {
		g_SpawnedNpcs[g_SpawnedNpcs.size()] = objId;
	} else if (lot == SPOUT_LOT) {
		g_Spouts[g_Spouts.size()] = objId;
	} else if (lot == BUBBLE_BLOWER_LOT) {
		g_BubbleBlowers[g_BubbleBlowers.size()] = objId;
	} else if (lot == POLE_SLIDE_NPC) {
		StoreParent(self, objId);
		StoreObjectByName(self, u"PoleSlideNPC", objId);
	} else if (lot == BALLOON_LOT) {
		StoreParent(self, objId);
		StoreObjectByName(self, u"Balloon", objId);
	}

	// print out all the objects
	for (const auto& [key, value] : g_SpawnedNpcs) {
		LOG("Npc %i %llu", key, value);
	}
	for (const auto& [key, value] : g_Spouts) {
		LOG("Spout %i %llu", key, value);
	}
	for (const auto& [key, value] : g_BubbleBlowers) {
		LOG("BubbleBlower %i %llu", key, value);
	}
}
