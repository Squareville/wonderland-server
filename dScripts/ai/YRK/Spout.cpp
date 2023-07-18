#include "Spout.h"

#include "Entity.h"
#include "EntityManager.h"
#include "SkillComponent.h"
#include "ZorilloContants.h"
#include "dLogger.h"
#include "Game.h"

bool Spout::GetEnableState(Entity* self) {
	// Game::logger->Log("Spout", "Getting enable state");
    return self->GetVar<bool>(u"SpoutEnabled");
}

void Spout::SetEnableState(Entity* self, SkunkEventState state) {
	// Game::logger->Log("Spout", "enable state %i", state);
    bool isEnabled = state == SkunkEventState::ZoneStateNoInvasion;
    self->SetVar<bool>(u"SpoutEnabled", isEnabled);
}

void Spout::OnStartup(Entity* self) {
	// Game::logger->Log("Spout", "starting up spout!");
    self->SetVar<bool>(u"PlayerOnMe", false);    //-- @TODO: leaveprox does not work on logout
    self->SetProximityRadius(ZorilloConstants::radius, "SpoutRadius");
    self->SetVar<bool>(u"SpoutEnabled", true); 
}

void Spout::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
	// Game::logger->Log("Spout", "proximity! (%s) (%s)", name.c_str(), status.c_str());
    if(status == "ENTER" && entering->IsPlayer() && !self->GetVar<bool>(u"PlayerOnMe")) {
		CleanPlayer(self, entering);
		self->SetVar<bool>(u"PlayerOnMe", true);
	} else if (status == "LEAVE" && entering->IsPlayer()) {
		self->CancelAllTimers();
		self->AddTimer("ProxCheck", 0.1f);
	}
}

void Spout::CleanPlayer(Entity* self, Entity* entering) {
	// Game::logger->Log("Spout", "Clean player %llu", entering->GetObjectID());
    if (GetEnableState(self)) {
		auto* skillComponent = self->GetComponent<SkillComponent>();
		if (skillComponent) skillComponent->CalculateBehavior(ZorilloConstants::destinkSkill, 252, entering->GetObjectID());
	}
}

void Spout::OnTimerDone(Entity* self, std::string timerName) {
    if (timerName == "ProxCheck") {
		Game::logger->Log("Spout", "Timer is done");
        if (!ArePlayersInProximity(self)) {
            self->SetVar<bool>(u"PlayerOnMe", false); 
		}
	}
}

bool Spout::ArePlayersInProximity(Entity* self) {
	auto& targetsInPhantom = self->GetTargetsInPhantom();
	for (auto& target : targetsInPhantom) {
		auto* possiblePlayer = Game::entityManager->GetEntity(target);
		if (possiblePlayer && possiblePlayer->IsPlayer()) {
			// Game::logger->Log("Spout", "A player is in proximity!"); Still needs to be tested.
			return true;
			}
	}
	return false;
}

void Spout::OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1, int32_t param2) {
	// Game::logger->Log("Spout", "Got notification %s with param1 %i", name.c_str(), param1);
    if (name == "zone_state_change") SetEnableState(self, static_cast<SkunkEventState>(param1));
}
