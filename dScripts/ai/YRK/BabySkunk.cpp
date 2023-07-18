#include "BabySkunk.h"

#include "Game.h"
#include "EntityManager.h"
#include "MissionComponent.h"
#include "eMissionState.h"
#include "Entity.h"
#include "MovementAIComponent.h"

void BabySkunk::OnStartup(Entity* self) {
	// self:FollowWaypoints()

	self->SetProximityRadius(15, "babySkunkRadius");

	self->SetVar<bool>(u"IsFollowing", false);
	self->SetVar<int32_t>(u"LinePosit", 0);
	self->SetVar<bool>(u"ReturningHome", false);

	// local pos = getHomePoint(self)
	// self:SetTetherPoint { tetherPt = self:GetPosition().pos, radius = 100 }
}

void BabySkunk::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
	if (self->GetVar<bool>(u"IsFollowing")) return;

	if (self->GetVar<bool>(u"ReturningHome") == true) return;

	if (status != "ENTER") return;

	if (!entering->IsPlayer()) return;
	auto* missionComponent = entering->GetComponent<MissionComponent>();
	if (!missionComponent) return;

	if (missionComponent->GetMissionState(133) == eMissionState::ACTIVE) {
		// Game::entityManager->GetZoneControlEntity()->OnRequestFollow(self, entering);
		bool canFollow = false;
		int32_t followPosition = 0;
		if (canFollow) {
			self->CancelAllTimers();
			self->SetVar<int32_t>(u"LinePosit", followPosition);
			auto* movementAIComponent = self->GetComponent<MovementAIComponent>();
			if (!movementAIComponent) return;

			auto mySpeed = movementAIComponent->GetSpeed();
			movementAIComponent->SetDestination(entering->GetPosition());
			mySpeed *= GeneralUtils::GenerateRandomNumber<int32_t>(2, 5);
			movementAIComponent->SetSpeed(mySpeed);
			// radius = FollowMsg.iPosit * 5,

			self->SetVar<bool>(u"IsFollowing", true);

			self->AddTimer("despawnTimer", 140); // Dont hardcode despawn time
		}

	} else {
		self->CancelAllTimers();
		// try to avoid the player
	}
}

void BabySkunk::OnTimerDone(Entity* self, std::string timerName) {

	if (timerName == "despawnTimer") {
		self->Smash(LWOOBJID_EMPTY, eKillType::SILENT);
	}

	if (timerName == "ReturnHome") {
		self->SetVar<bool>(u"ReturningHome", true);
		auto* movementAIComponent = self->GetComponent<MovementAIComponent>();
		if (!movementAIComponent) return;

		movementAIComponent->SetSpeed(10);
		// self:FollowWaypoints()
	}
}

void BabySkunk::OnArrived(Entity* self) {
	auto* movementAIComponent = self->GetComponent<MovementAIComponent>();
	if (!movementAIComponent) return;

	movementAIComponent->SetSpeed(1);
	self->SetVar<bool>(u"ReturningHome", false);
	// follow waypoints
}

void BabySkunk::OnStoppedEvading(Entity* self) {
	self->AddTimer("ReturnHome", 10);
}

void BabySkunk::OnLeftTetherRadius(Entity* self) {
	if (self->GetVar<bool>(u"IsFollowing")) return;

	self->SetVar<bool>(u"ReturningHome", true);
	// self->FollowWaypoints()
	auto* movementAIComponent = self->GetComponent<MovementAIComponent>();
	if (!movementAIComponent) return;

	movementAIComponent->SetSpeed(10);

}
