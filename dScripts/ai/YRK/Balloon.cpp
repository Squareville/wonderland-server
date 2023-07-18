#include "Balloon.h"

#include "DestroyableComponent.h"
#include "Entity.h"
#include "GameMessages.h"
#include "MovingPlatformComponent.h"
#include "MissionComponent.h"
#include "eMissionTaskType.h"
#include "ProximityMonitorComponent.h"
#include "SkillComponent.h"
#include "ZorilloContants.h"

#include "Game.h"
#include "dLogger.h"

void Balloon::OnStartup(Entity* self) {
	Game::logger->Log("Balloon", "starting up %i", self->GetLOT());
	auto* movingPlatformComponent = self->GetComponent<MovingPlatformComponent>();
	if (movingPlatformComponent) {
		Game::logger->Log("Balloon", "stopping platform");
		movingPlatformComponent->StopPathing();
		movingPlatformComponent->WarpToWaypoint(0);
	}
	ResetStink(self);

	self->SetVar<bool>(u"balloonInUse", false);

	self->SetVar<bool>(u"playerOnBalloon", false);
}

void Balloon::OnFireEventServerSide(Entity* self, Entity* sender, std::string args, int32_t param1, int32_t param2, int32_t param3) {
	Game::logger->Log("Balloon", "firing event %s", args.c_str());
	if (args == "switchOn") self->SetVar<LWOOBJID>(u"triggerID", sender->GetObjectID());

	if (self->GetVar<bool>(u"balloonInUse")) return;

	CheckForLaunch(self);
}

void Balloon::OnTimerDone(Entity* self, std::string timerName) {
	Game::logger->Log("Balloon", "timer %s finished", timerName.c_str());
	if (timerName == "recheckTimer") {
		CheckForLaunch(self);
	} else if (timerName == "CreditCheckTimer") {
		GiveCreditForLaunching(self);
	}

}

void Balloon::ResetStink(Entity* self) {
	Game::logger->Log("Balloon", "resetting stink");
	self->SetVar<uint32_t>(u"currentStink", 0);
}

void Balloon::SetStinkToMax(Entity* self) {
	Game::logger->Log("Balloon", "setting stink to max");
	self->SetVar<uint32_t>(u"currentStink", sufficientStink);
}

void Balloon::CheckForLaunch(Entity* self) {
	Game::logger->Log("Balloon", "checking for launch");
	auto previousStink = self->GetVar<uint32_t>(u"currentStink");

	ResetStink(self);
	self->SetVar<bool>(u"blayerOnBalloon", false);

	UpdateStinkAtSwitch(self);

	auto newStink = self->GetVar<uint32_t>(u"currentStink");
	if (newStink != previousStink) UpdateAnimation(self, newStink);

	if (newStink >= sufficientStink) {
		LaunchBalloon(self);
	} else if (self->GetVar<bool>(u"playerOnBalloon")) {
		SetRecheckTimer(self);
	}
}

void Balloon::LaunchBalloon(Entity* self) {
	Game::logger->Log("Balloon", "launching balloon");
	self->SetVar<bool>(u"balloonInUse", true);

	auto* movingPlatformComponent = self->GetComponent<MovingPlatformComponent>();
	if (movingPlatformComponent) movingPlatformComponent->StartPathing();

	self->AddTimer("CreditCheckTimer", 0.1);
}

void Balloon::UpdateStinkAtSwitch(Entity* self) {
	Game::logger->Log("Balloon", "updating stink at switch");
	auto triggerID = self->GetVar<LWOOBJID>(u"triggerID");
	auto* triggerEntity = Game::entityManager->GetEntity(triggerID);
	// triggerID = getObjectByName(self, "triggerID")

	if (!triggerEntity) return;
	auto* proximityMonitorComponent = triggerEntity->GetComponent<ProximityMonitorComponent>();
	if (!proximityMonitorComponent) return;

	auto objs = proximityMonitorComponent->GetProximityObjects("balloon_radius");

	uint32_t stinkAmount = 0;

	for (auto pair : objs) {
		auto* targetEntity = Game::entityManager->GetEntity(pair.first);
		if (!targetEntity) continue;
		auto* destroyableComponent = targetEntity->GetComponent<DestroyableComponent>();
		if (!destroyableComponent) continue;
		auto hasFaction = destroyableComponent->HasFaction(1);

		if (hasFaction) {
			self->SetVar<bool>(u"bPlayerOnBalloon", true);
			auto* skillComponent = targetEntity->GetComponent<SkillComponent>();
			if (!skillComponent) continue;

			if (skillComponent->HasSkill(imitationSkunkSkill)) {
				SetStinkToMax(self);
				return;
			}

			if (skillComponent->HasSkill(skunkStinkSkill)) {
				stinkAmount = stinkAmount + 1;
				if (stinkAmount >= sufficientStink) {
					SetStinkToMax(self);
					return;
				}
			}

		}
	}

	self->SetVar<bool>(u"currentStink", stinkAmount);
}

void Balloon::SetRecheckTimer(Entity* self) {
	Game::logger->Log("Balloon", "set recheck timer");
	self->CancelTimer("recheckTimer");
	self->AddTimer("recheckTimer", recheckTime);
}

void Balloon::GiveCreditForLaunching(Entity* self) {
	Game::logger->Log("Balloon", "give credit for launching");
	auto triggerID = self->GetVar<LWOOBJID>(u"triggerID");
	auto* triggerEntity = Game::entityManager->GetEntity(triggerID);
	if (!triggerEntity) return;

	auto* proximityMonitorComponent = triggerEntity->GetComponent<ProximityMonitorComponent>();
	if (!proximityMonitorComponent) return;
	auto objs = proximityMonitorComponent->GetProximityObjects("balloon_radius");

	for (auto pair : objs) {
		bool skunkStink = false;
		bool imitationStink = false;

		auto* targetEntity = Game::entityManager->GetEntity(pair.first);
		if (!targetEntity) continue;

		auto* destroyableComponent = targetEntity->GetComponent<DestroyableComponent>();
		if (!destroyableComponent) continue;

		bool hasFaction = destroyableComponent->HasFaction(1);

		if (hasFaction) {
			auto* skillComponent = targetEntity->GetComponent<SkillComponent>();
			if (!skillComponent) continue;

			if (skillComponent->HasSkill(skunkStinkSkill)) skunkStink = true;

			if (skillComponent->HasSkill(imitationSkunkSkill)) imitationStink = true;

			if (skunkStink || imitationStink) {
				auto* missionComponent = targetEntity->GetComponent<MissionComponent>();
				if (missionComponent) missionComponent->Progress(eMissionTaskType::SMASH, self->GetLOT());
			}

		}
	}

}

void Balloon::UpdateAnimation(Entity* self, uint32_t stinkAmount) {
	Game::logger->Log("Balloon", "updating animation %i", stinkAmount);
	if (self->GetVar<bool>(u"renderReady") == false) return;

	if (stinkAmount == 0) {
		GameMessages::SendPlayAnimation(self, u"balloon1");
	} else if (stinkAmount == 1) {
		GameMessages::SendPlayAnimation(self, u"balloon2");
	} else if (stinkAmount == sufficientStink) {
		GameMessages::SendPlayAnimation(self, u"balloon3");
	}
}

void Balloon::OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1, int32_t param2) {
	Game::logger->Log("Balloon", "rip function");
	// if (msg.name == "playerLoaded") {		
	// 	self:NotifyClientRebuildSectionState { rerouteID = msg.ObjIDSender, iState = self:GetVar("currentStink") }
	// }
}

void Balloon::OnWaypointReached(Entity* self, uint32_t waypointIndex) {
	Game::logger->Log("Balloon", "waypoint reached %i", waypointIndex);
	if (waypointIndex == ZorilloConstants::lastBalloonWaypoint) {

		auto* movingPlatformComponent = self->GetComponent<MovingPlatformComponent>();
		if (movingPlatformComponent) movingPlatformComponent->StopPathing();

		self->SetVar<bool>(u"balloonInUse", false);

		CheckForLaunch(self);
	}

}
