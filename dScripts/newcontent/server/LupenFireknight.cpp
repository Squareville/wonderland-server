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
}

void LupenFireknight::OnHit(Entity* self, Entity* attacker) {
	auto* destroyableComponent = self->GetComponent<DestroyableComponent>();
	auto* skillComponent = self->GetComponent<SkillComponent>();

	if (destroyableComponent == nullptr || skillComponent == nullptr) {
		return;
	}

	if (destroyableComponent->GetHealth() < 12 && !self->GetVar<bool>(u"injured")) {
		self->SetVar(u"injured", true);

		skillComponent->CastSkill(2276, self->GetObjectID());

		const auto attackerID = attacker->GetObjectID();

		self->AddCallbackTimer(4.5f, [this, self, attackerID]() {
			self->Smash(attackerID);
			});
	}
}
