#include "SkillCastAndOptionalDeath.h"

#include "Entity.h"
#include "SkillComponent.h"

void SkillCastAndOptionalDeath::OnStartup(Entity* self) {
	auto* skillComponent = self->GetComponent<SkillComponent>();
	if (!skillComponent) return;
	skillComponent->CastSkill(m_SkillToCast, self->GetObjectID());
	if (m_DieAfterXSeconds) self->AddCallbackTimer(m_TimeToDie, [self]() { self->Smash(); });
}
