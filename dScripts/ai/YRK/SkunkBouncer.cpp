#include "SkunkBouncer.h"

#include "Entity.h"
#include "GameMessages.h"
#include "SkillComponent.h"

void SkunkBouncer::CheckForStink(Entity* self, Entity* target) {
	auto* skillComponent = target->GetComponent<SkillComponent>();
	bool addStink = false;
	if (IsTargetABabySkunk(target)) {
		addStink = true;
	} else if (IsTargetAnAdultSkunk(target) && skillComponent && skillComponent->HasSkill(STINKY_SKUNK_SKILL)) {
		addStink = true;
	}

	if (addStink) {
		AddStinkEffectToBouncer(self);
		AddStinkEffectToTarget(target);
	}
}

void SkunkBouncer::AddStinkEffectToBouncer(Entity* target) {
	GameMessages::SendPlayFXEffect(target->GetObjectID(), m_BouncerStinkFxEffectId, m_SkunkEffectName, "", LWOOBJID_EMPTY, 1.1f);
}

void SkunkBouncer::AddStinkEffectToTarget(Entity* target) {
	if (IsTargetABabySkunk(target)) {
		GameMessages::SendPlayFXEffect(target->GetObjectID(), m_SkunkStinkFxEffectId, m_BabySkunkEffectName, "", LWOOBJID_EMPTY, 1.1f);
	} else if (IsTargetAnAdultSkunk(target)) {
		GameMessages::SendPlayFXEffect(target->GetObjectID(), m_SkunkStinkFxEffectId, m_AdultSkunkEffectName, "", LWOOBJID_EMPTY, 1.1f);
	}
}

void SkunkBouncer::OnCollisionPhantom(Entity* self, Entity* target) {
	BounceNow(self, target);
}

void SkunkBouncer::BounceNow(Entity* self, Entity* target) {
	if (IsTargetASkunk(self, target)) {
		// Do bouncer stuff
		CheckForStink(self, target);	
	}
}

bool SkunkBouncer::IsTargetASkunk(Entity* self, Entity* target) {
	return IsTargetABabySkunk(target) || IsTargetAnAdultSkunk(target);
}

bool SkunkBouncer::IsTargetABabySkunk(Entity* target) {
	return target->GetLOT() == m_BabySkunk;
}

bool SkunkBouncer::IsTargetAnAdultSkunk(Entity* target) {
	auto candidate = target->GetLOT();
	for (auto lot : m_AdultSkunk) {
		if (lot == candidate) return true;
	}
	return false;
}
