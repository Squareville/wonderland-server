#include "BubbleStatue.h"

#include "ControllablePhysicsComponent.h"
#include "DestroyableComponent.h"
#include "Entity.h"
#include "GameMessages.h"
#include "SkillComponent.h"
#include "ZorilloContants.h"

void BubbleStatue::OnStartup(Entity* self) {
	self->SetProximityRadius(m_BubbleStatueRadius, "BUBBLE_STATUE_RADIUS");
	self->SetVar<bool>(u"StatueEnabled", true);
}

void BubbleStatue::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
	auto* destroyableComponent = entering->GetComponent<DestroyableComponent>();
	if (!destroyableComponent) return;

	// Player faction is 1
	if (name == "BUBBLE_STATUE_RADIUS" && status == "ENTER" && !self->GetVar<bool>(u"StatueEnabled") && destroyableComponent->HasFaction(1)) {
		auto* skillComponent = self->GetComponent<SkillComponent>();
		if (!skillComponent) return;
		skillComponent->CalculateBehavior(ZorilloConstants::destinkSkill, 252, entering->GetObjectID());
		auto* controllablePhysicsComponent = entering->GetComponent<ControllablePhysicsComponent>();
		if (controllablePhysicsComponent) controllablePhysicsComponent->ActivateBubbleBuff(eBubbleType::SKUNK);
	}
}

void BubbleStatue::OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1, int32_t param2) {
	Game::logger->Log("BubbleStatue", "state is now %s %i", name.c_str(), param1);
	if (name == "zone_state_change") {
		self->SetVar<bool>(u"StatueEnabled", static_cast<SkunkEventState>(param1) == SkunkEventState::ZoneStateNoInvasion);
	}
}