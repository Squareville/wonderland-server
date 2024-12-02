#include "EnemySkunk.h"

#include "ControllablePhysicsComponent.h"
#include "MovementAIComponent.h"

void EnemySkunk::OnSkillEventFired(Entity* self, Entity* caster, const std::string& message) {
	if (message != "waterspray") return;

	auto* const movementAiComponent = self->GetComponent<MovementAIComponent>();
	if (movementAiComponent) movementAiComponent->Stop();

	auto* const controllablePhysicsComponent = self->GetComponent<ControllablePhysicsComponent>();
	if (controllablePhysicsComponent) {
		controllablePhysicsComponent->ActivateBubbleBuff(eBubbleType::SKUNK);
		controllablePhysicsComponent->SetVelocity(NiPoint3(0.0f, 5.0f, 0.0f));
	}

	Game::entityManager->SerializeEntity(self);
	const auto casterId = caster->GetObjectID();
	self->AddCallbackTimer(4.0f, [self, casterId]() {
		if (!self) return;
		auto* controllablePhysicsComponent = self->GetComponent<ControllablePhysicsComponent>();
		if (controllablePhysicsComponent) {
			controllablePhysicsComponent->DeactivateBubbleBuff();
			controllablePhysicsComponent->SetVelocity(NiPoint3Constant::ZERO);
			Game::entityManager->SerializeEntity(self);
		}

		Game::entityManager->GetZoneControlEntity()->NotifyObject(self, self->GetVar<std::string>(u"respawn_path"));
		self->Smash(casterId);
		});
}
