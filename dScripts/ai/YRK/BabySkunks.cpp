#include "BabySkunks.h"
#include "Entity.h"
#include "eMissionState.h"
#include "MissionComponent.h"
#include "MovementAIComponent.h"

void BabySkunks::OnStartup(Entity* self) {
	self->SetProximityRadius(15.0f, "BabySkunk");
	self->SetVar<int32_t>(u"IsFollowing", 0);
	self->SetVar<int32_t>(u"ReturningHome", 0);
	auto* const movementAiComponent = self->GetComponent<MovementAIComponent>();
	if (!movementAiComponent) return;

	// movementAiComponent->SetTetherPoint(self->GetDefaultPosition(), 100.0f);
}

void BabySkunks::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
	if (self->GetVar<int32_t>(u"IsFollowing") == 1 ||
		self->GetVar<int32_t>(u"ReturningHome") == 1 ||
		status != "ENTER" ||
		!entering ||
		!entering->IsPlayer()) return;
	const auto* const missionComponent = entering->GetComponent<MissionComponent>();
	if (!missionComponent) return;
	
	const auto* const mission = missionComponent->GetMission(133);
	if (!mission) return;

	if (mission->GetMissionState() == eMissionState::ACTIVE) {
		self->CancelAllTimers();
		self->SetVar<int32_t>(u"IsFollowing", 1);
	} else {
		self->CancelAllTimers();
		
	}
}
