#include "HydrantQb.h"

#include "eRebuildState.h"
#include "RebuildComponent.h"
#include "SkillComponent.h"

bool IsActive(Entity* self) {
	auto* rebuildComponent = self->GetComponent<RebuildComponent>();
	if (!rebuildComponent) return false;
	auto rebuildState = rebuildComponent->GetState();
    
    return rebuildState == eRebuildState::OPEN;
}

void HydrantQb::OnUse(Entity* self, Entity* user) {
	auto* rebuildComponent = self->GetComponent<RebuildComponent>();
	if (!rebuildComponent) return;
    
    if (IsActive(self)) rebuildComponent->ResetRebuild(true);
}

void HydrantQb::OnRebuildNotifyState(Entity* self, eRebuildState state) {
    if (state == eRebuildState::RESETTING) {
		self->CancelAllTimers();
	} else if (state == eRebuildState::OPEN) {
		self->CancelAllTimers();
		// hydrant just got repaired to turn the water off again
	}
}

void CleanPlayer(Entity* self, Entity* target) {
	if (!target->IsPlayer()) return;
	auto* skillComponent = self->GetComponent<SkillComponent>();
	if (!skillComponent) return;
	skillComponent->CastSkill(116, target->GetObjectID());
	
}

void HydrantQb::OnFireEventServerSide(Entity* self, Entity* sender, std::string args, int32_t param1, int32_t param2, int32_t param3) {
	if (args != "cleanPlayer") return;
	CleanPlayer(self, sender);
}
