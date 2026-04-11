#include "Balloon.h"
#include "Entity.h"
#include "PhantomPhysicsComponent.h"
#include "GameMessages.h"
#include "RenderComponent.h"
#include "ProximityMonitorComponent.h"
#include "MissionComponent.h"
#include "BuffComponent.h"
#include "eMissionTaskType.h"

void ResetStink(Entity& self) {
	self.SetVar<int32_t>(u"currentStink", 0);
}

void Balloon::UpdateAnimation(Entity& self) {
	const auto stink = self.GetVar<int32_t>(u"currentStink");

	auto* renderComponent = self.GetComponent<RenderComponent>();
	if (renderComponent != nullptr) {
		renderComponent->StopEffect("balloonstink");
	}
	
	if (stink == 0) {
		RenderComponent::PlayAnimation(&self, "balloon1");
	} else if (stink == 1) {
		RenderComponent::PlayAnimation(&self, "balloon2");
	} else if (stink >= 2) {
		RenderComponent::PlayAnimation(&self, "balloon3");
		if (renderComponent != nullptr) {
			renderComponent->PlayEffect(20212, u"stink", "balloonstink");
		}
	}
}

void Balloon::LaunchBalloon(Entity& self) {
	
	self.SetVar(u"bBalloonInUse", true);
	GameMessages::SendPlatformResync(&self, UNASSIGNED_SYSTEM_ADDRESS, true, 0, 11, 1, eMovementPlatformState::Moving);
	self.AddTimer("FinishedPathing", m_PathingTime);
	self.AddTimer("CreditCheckTimer", 0.1f);
}

void Balloon::UpdateStinkAtSwitch(Entity& self) {
	const auto triggerID = GetEntityByName(&self, u"triggerID");
	if (!triggerID) return;

	const auto* const proximityMonitorComponent = triggerID->GetComponent<ProximityMonitorComponent>();
	if (!proximityMonitorComponent) return;

	int32_t stink = 0;
	for (const auto id : proximityMonitorComponent->GetProximityObjects("BalloonTrigger")) {
		const auto* const entity = Game::entityManager->GetEntity(id);
		if (!entity || !entity->IsPlayer()) continue;

		self.SetVar(u"bPlayerOnBalloon", true);
		const auto* const buffComponent = entity->GetComponent<BuffComponent>();
		if (!buffComponent) continue;
		
		// imitation
		if (buffComponent->HasBuff(m_ImitationSkunkStinkSkill)) {
			stink = m_SufficientStink;
		} else if (buffComponent->HasBuff(m_SkunkStinkSkill) || buffComponent->HasBuff(m_HeldItemSkunkStinkSkill)) {
			stink++;
		}
		if (stink >= m_SufficientStink) {
			SetStinkToMax(self);
			break;
		}
	}
	self.SetVar<int32_t>(u"currentStink", stink);
}

void Balloon::GiveCreditForLaunching(Entity& self) {
	const auto triggerID = GetEntityByName(&self, u"triggerID");
	if (!triggerID) return;

	const auto* const proximityMonitorComponent = triggerID->GetComponent<ProximityMonitorComponent>();
	if (!proximityMonitorComponent) return;

	for (const auto id : proximityMonitorComponent->GetProximityObjects("BalloonTrigger")) {
		const auto* const entity = Game::entityManager->GetEntity(id);
		if (!entity || !entity->IsPlayer()) continue;
		
		auto* const missionComponent = entity->GetComponent<MissionComponent>();
		if (missionComponent) {
			missionComponent->Progress(eMissionTaskType::SCRIPT, self.GetLOT());
		}
	}
}

void Balloon::SetStinkToMax(Entity& self) {
	self.SetVar<int32_t>(u"currentStink", m_SufficientStink);
}

void Balloon::CheckForLaunch(Entity& self) {
	
	const auto prevStink = self.GetVar<int32_t>(u"currentStink");
	ResetStink(self);
	self.SetVar<bool>(u"bPlayerOnBalloon", false);

	UpdateStinkAtSwitch(self);

	const auto newStink = self.GetVar<int32_t>(u"currentStink");
	if (prevStink != newStink) {
		UpdateAnimation(self);
	}

	if (newStink >= m_SufficientStink) {
		LaunchBalloon(self);
	} else if (self.GetVar<bool>(u"bPlayerOnBalloon")) {
		self.CancelTimer("recheckTimer");
		self.AddTimer("recheckTimer", m_RecheckTime);
	}
}

void Balloon::OnStartup(Entity* self) {
	
	ResetStink(*self);
	self->SetVar<bool>(u"bBalloonInUse", false);
	self->SetVar<bool>(u"bPlayerOnBalloon", false);
	Game::entityManager->GetZoneControlEntity()->OnObjectLoaded(self->GetObjectID(), self->GetLOT());
}

void Balloon::OnPlayerLoaded(Entity* self, Entity* player) {
	// make sure the balloon is still stationary for players who load in after it has already stopped
	GameMessages::SendPlatformResync(self, player->GetSystemAddress(), true, 0, 0, 0, eMovementPlatformState::Stationary);
}

void Balloon::OnFireEventServerSide(Entity* self, Entity* sender, std::string args, int32_t param1, int32_t param2, int32_t param3) {
	
	LOG("Balloon received event %s with params %i, %i, %i", args.c_str(), param1, param2, param3);
	if (args == "switchOn") {
		StoreEntityByName(self, u"triggerID", sender->GetObjectID());
	}

	if (self->GetVar<bool>(u"bBalloonInUse")) return;
	CheckForLaunch(*self);
}

void Balloon::OnTimerDone(Entity* self, std::string name) {
	if (name == "recheckTimer") {
		CheckForLaunch(*self);
	} else if (name == "CreditCheckTimer") {
		GiveCreditForLaunching(*self);
	} else if (name == "FinishedPathing") {
		self->SetVar<bool>(u"bBalloonInUse", false);
		CheckForLaunch(*self);
	}
}
