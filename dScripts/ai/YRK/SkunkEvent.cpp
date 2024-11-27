#include "SkunkEvent.h"

namespace {
	std::set<int32_t> g_InvasionStinkCloudWaypoints{};
	std::map<LWOOBJID, int32_t> g_PlayerPoints{};
};

SkunkEventZoneState SkunkEvent::GetZoneState(const Entity* const self) const {
	return static_cast<SkunkEventZoneState>(self->GetVar<int32_t>(u"ZoneState"));
}

bool SkunkEvent::InvasionActive(const Entity* const self) const {
	using enum SkunkEventZoneState;
	const auto zoneState = GetZoneState(self);
	return zoneState == TRANSITION || zoneState == HIGH_ALERT || zoneState == MEDIUM_ALERT || zoneState == LOW_ALERT;
}

bool SkunkEvent::IncrementTotalCleanPoints(Entity* const self, const int32_t points) const {
	using enum SkunkEventZoneState;
	if (!InvasionActive(self)) return false;

	auto totalCleanPoints = self->GetVar<int32_t>(u"TotalCleanPoints");
	totalCleanPoints += points;
	self->SetVar<int32_t>(u"TotalCleanPoints", totalCleanPoints);

	const auto state = GetZoneState(self);
	if (state == HIGH_ALERT && totalCleanPoints >= CLEANING_POINTS_MEDIUM) {
		self->SetVar(u"ZoneState", GeneralUtils::ToUnderlying(MEDIUM_ALERT));
	} else if (state == MEDIUM_ALERT && totalCleanPoints >= CLEANING_POINTS_LOW) {
		self->SetVar(u"ZoneState", GeneralUtils::ToUnderlying(LOW_ALERT));
	} else if (state == LOW_ALERT && totalCleanPoints >= CLEANING_POINTS_TOTAL) {
		self->SetVar(u"ZoneState", GeneralUtils::ToUnderlying(DONE_TRANSITION));
		return true;
	}

	return false;
}

void SkunkEvent::OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1, int32_t param2) {
	if (!InvasionActive(self)) return;

	if (name == "skunk_cleaned") {

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

void SkunkEvent::SpawnSingleStinkCloud(Entity* const self, const int32_t number) const {
	const auto maxPoints = self->GetVar<int32_t>(u"NumStinkCloudSpawnPoints");
	if (maxPoints == 0) return;
	int32_t maxTries = 20;
	int32_t waypoint{};
	do {
		waypoint = GeneralUtils::GenerateRandomNumber<int32_t>(1, maxPoints);
		if (maxTries-- == 0) return;
	} while (IsValidWaypoint(self, waypoint));
	g_InvasionStinkCloudWaypoints.insert(waypoint);

	const auto* const path = Game::zoneManager->GetZone()->GetPath(STINK_CLOUD_PATH);
	if (!path || waypoint > path->pathWaypoints.size()) return;

	EntityInfo info{};
	info.lot = INVASION_STINK_CLOUD_LOT;
	info.settings = { new LDFData<int32_t>(u"StinkCloudNum", number) };
	info.pos = path->pathWaypoints[waypoint - 1].position;
	
	auto* const newStinkCloud = Game::entityManager->CreateEntity(info, nullptr, self);
	Game::entityManager->ConstructEntity(newStinkCloud);
}

bool SkunkEvent::IsValidWaypoint(const Entity* const self, const int32_t waypoint) const {
	if (waypoint < 1) return false;
	return !g_InvasionStinkCloudWaypoints.contains(waypoint);
}

void SkunkEvent::AddPlayerPoints(const Entity* const self, const LWOOBJID player, const int32_t points) const {
	if (!InvasionActive(self) || player == LWOOBJID_EMPTY || points <= 0) return;
	g_PlayerPoints[player] += points;
}
