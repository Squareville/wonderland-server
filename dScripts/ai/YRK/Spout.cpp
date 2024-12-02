#include "Spout.h"
#include "Entity.h"
#include "Game.h"
#include "EntityManager.h"
#include "SkillComponent.h"
#include "ProximityMonitorComponent.h"
#include "SkunkEvent.h"

void Spout::OnStartup(Entity* self) {
	Game::entityManager->GetZoneControlEntity()->OnObjectLoaded(self->GetObjectID(), self->GetLOT());
	self->SetVar<bool>(u"PlayerOnMe", false);
	self->SetProximityRadius(3.0f, "Spout");
	self->SetVar<bool>(u"SpoutEnabled", true);
}

void Spout::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
	if (name != "Spout") return;
	if (status == "ENTER" && entering->IsPlayer() && !self->GetVar<bool>(u"PlayerOnMe")) {
		if (self->GetVar<bool>(u"SpoutEnabled")) {
			auto* const skillComponent = self->GetComponent<SkillComponent>();
			if (!skillComponent) return;

			skillComponent->CastSkill(116, entering->GetObjectID()); // DESTINK_SKILL
		}
		self->SetVar<bool>(u"PlayerOnMe", true);
	} else if (status == "LEAVE" && entering->IsPlayer()) {
		self->CancelAllTimers();
		const auto* const proximityMonitorComponent = self->GetComponent<ProximityMonitorComponent>();
		if (!proximityMonitorComponent) return;

		const auto proxObjs = proximityMonitorComponent->GetProximityObjects("Spout");
		bool foundPlayer = false;
		for (const auto id : proxObjs) {
			const auto* const entity = Game::entityManager->GetEntity(id);
			if (entity && entity->IsPlayer()) {
				foundPlayer = true;
				break;
			}
		}
		self->SetVar(u"PlayerOnMe", foundPlayer);
	}
}

void Spout::OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1, int32_t param2) {
	if (name == "zone_state_change") {
		self->SetVar<bool>(u"SpoutEnabled", param1 == GeneralUtils::ToUnderlying(SkunkEventZoneState::NO_INVASION));
	}
}
