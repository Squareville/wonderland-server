#include "MechEuBroombot.h"

#include "MovementAIComponent.h"
#include "SkillComponent.h"
#include "dZoneManager.h"
#include "SkunkEvent.h"

void MechEuBroombot::OnUpdate(Entity* self) {
	// This is omega bodge but i cant really do anything better at this time
	for (auto* const entity : Game::entityManager->GetEntitiesByProximity(self->GetPosition(), 3.0f)) {
		if (!entity || entity->GetLOT() != SkunkEvent::INVASION_STINK_CLOUD_LOT || entity->GetVar<bool>(u"cleaned")) continue;
		auto const [skillComponent, movementAiComponent] = self->GetComponentsMut<SkillComponent, MovementAIComponent>();
		if (skillComponent) {
			skillComponent->CastSkill(115, entity->GetObjectID());
			entity->SetVar<bool>(u"cleaned", true);
		}
		if (movementAiComponent) {
			movementAiComponent->Pause(5.0f);
		}
	}
}

void MechEuBroombot::OnUse(Entity* self, Entity* builder) {
	if (!builder) return;

	const auto [movementAiComponent, skillComponent] = self->GetComponentsMut<MovementAIComponent, SkillComponent>();
	if (movementAiComponent) {
		movementAiComponent->Pause(5.0f);
	}

	if (skillComponent) {
		skillComponent->CastSkill(115, builder->GetObjectID());
	}
}

void MechEuBroombot::OnQuickBuildComplete(Entity* self, Entity* builder) {
	if (!builder) return;

	StoreEntityByName(self, u"playerBuilder", builder->GetObjectID());
	Game::zoneManager->GetZoneControlObject()->NotifyObject(self, "broombot_fixed");
}
