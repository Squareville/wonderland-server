#include "BubbleStatue.h"
#include "Game.h"
#include "EntityManager.h"
#include "ControllablePhysicsComponent.h"
#include "SkillComponent.h"

void BubbleStatue::OnStartup(Entity* self) {
	Game::entityManager->GetZoneControlEntity()->OnObjectLoaded(self->GetObjectID(), self->GetLOT());
	self->SetProximityRadius(10.0f, "BubbleStatue"); // BUBBLE_STATUE_RADIUS
	self->SetVar(u"StatueEnabled", true);
}

void BubbleStatue::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
	if (name != "BubbleStatue" || status != "ENTER" || !entering->IsPlayer() || !self->GetVar<bool>(u"StatueEnabled")) return;

	auto* const skillComponent = self->GetComponent<SkillComponent>();
	if (!skillComponent) return;
	skillComponent->CastSkill(116, entering->GetObjectID()); // DESTINK_SKILL

	auto* const controllablePhysicsComponent = entering->GetComponent<ControllablePhysicsComponent>();
	if (!controllablePhysicsComponent) return;
	controllablePhysicsComponent->ActivateBubbleBuff(eBubbleType::DEFAULT);
}

void BubbleStatue::OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1, int32_t param2) {
	if (name == "zone_state_change") self->SetVar(u"StatueEnabled", param1 == 0); // ZONE_STATE_NO_INVASION
}
