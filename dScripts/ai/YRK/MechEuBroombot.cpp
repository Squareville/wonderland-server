#include "MechEuBroombot.h"

#include "MovementAIComponent.h"
#include "QuickBuildComponent.h"
#include "SkillComponent.h"
#include "dZoneManager.h"
#include "SkunkEvent.h"

void MechEuBroombot::OnUpdate(Entity* self) {
	auto const [skillComponent, movementAiComponent, quickbuildComponent] = self->GetComponentsMut<SkillComponent, MovementAIComponent, QuickBuildComponent>();

	if (!skillComponent || !movementAiComponent || !quickbuildComponent || quickbuildComponent->GetState() != eQuickBuildState::COMPLETED) return;

	// This is omega bodge but i cant really do anything better at this time
	for (auto* const entity : Game::entityManager->GetEntitiesByProximity(self->GetPosition(), 6.0f)) {
		if (!entity || entity->GetLOT() != SkunkEvent::INVASION_STINK_CLOUD_LOT || entity->GetVar<bool>(u"cleaned")) continue;
		skillComponent->CastSkill(115, entity->GetObjectID());
		entity->SetVar<bool>(u"cleaned", true);
		movementAiComponent->Pause(5.0f);
	}
}

void MechEuBroombot::OnUse(Entity* self, Entity* builder) {
	if (!builder) return;
	auto const [skillComponent, movementAiComponent, quickbuildComponent] = self->GetComponentsMut<SkillComponent, MovementAIComponent, QuickBuildComponent>();

	if (!skillComponent || !movementAiComponent || !quickbuildComponent || quickbuildComponent->GetState() != eQuickBuildState::COMPLETED) return;

	movementAiComponent->Pause(5.0f);
	skillComponent->CastSkill(115, builder->GetObjectID());
}

void MechEuBroombot::OnQuickBuildComplete(Entity* self, Entity* builder) {
	if (!builder) return;

	StoreEntityByName(self, u"playerBuilder", builder->GetObjectID());
	Game::zoneManager->GetZoneControlObject()->NotifyObject(builder, "broombot_fixed");
}
