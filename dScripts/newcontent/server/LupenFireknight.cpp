#include "LupenFireknight.h"
#include "DestroyableComponent.h"
#include "ControllablePhysicsComponent.h"
#include "eStateChangeType.h"
#include "SkillComponent.h"

void LupenFireknight::OnStartup(Entity* self) {
	auto* destroyableComponent = self->GetComponent<DestroyableComponent>();
	if (destroyableComponent) {
		destroyableComponent->SetStatusImmunity(
			eStateChangeType::PUSH, false, false, true, true, false, false, false, false, true
		);
	}

	auto* controllablePhysicsComponent = self->GetComponent<ControllablePhysicsComponent>();
	if (controllablePhysicsComponent) {
		controllablePhysicsComponent->SetStunImmunity(
			eStateChangeType::PUSH, self->GetObjectID(), true
		);
	}

	auto* skillComponent = self->GetComponent<SkillComponent>();
	if (skillComponent) {
		// 999 damage reduction
		skillComponent->CastSkill(1996, self->GetObjectID());
	}
}

void LupenFireknight::OnHit(Entity* self, Entity* attacker) {
	auto* destroyableComponent = self->GetComponent<DestroyableComponent>();
	auto* skillComponent = self->GetComponent<SkillComponent>();

	if (destroyableComponent == nullptr || skillComponent == nullptr) {
		return;
	}

	if (destroyableComponent->GetHealth() < 15 && !self->GetVar<bool>(u"injured")) {
		self->SetVar(u"injured", true);

		// boom
		skillComponent->CastSkill(2276, self->GetObjectID());

		const auto attackerID = attacker->GetObjectID();

		self->AddCallbackTimer(8.5f, [this, self, attackerID]() {
			self->Smash(attackerID);
			});
	}
}
