#include "SkillCast.h"

#include "Entity.h"
#include "SkillComponent.h"

void SkillCast::OnStartup(Entity* self) {
	auto* skillComponent = self->GetComponent<SkillComponent>();
	if (!skillComponent) return;
	skillComponent->CastSkill(m_SkillToCast, self->GetObjectID());
}
