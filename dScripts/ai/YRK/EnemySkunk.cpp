#include "EnemySkunk.h"

#include "ControllablePhysicsComponent.h"
#include "MovementAIComponent.h"
#include "SkillComponent.h"

void EnemySkunk::OnStartup(Entity* self) {
	self->SetProximityRadius(3.0f, "StinkyPlayer");
}

void EnemySkunk::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
	if (!entering || status != "ENTER" || name != "StinkyPlayer") return;

	if (GeneralUtils::GenerateRandomNumber<uint32_t>(0, 100) <= 30) {
		auto* skillComponent = self->GetComponent<SkillComponent>();
		if (!skillComponent) return;

		skillComponent->CastSkill(772, entering->GetObjectID());
	}
}

void EnemySkunk::OnSkillEventFired(Entity* self, Entity* caster, const std::string& message) {
	if (message != "waterspray" && message != "soapspray") return;

	auto* const movementAiComponent = self->GetComponent<MovementAIComponent>();
	if (movementAiComponent) movementAiComponent->Stop();

	auto* const controllablePhysicsComponent = self->GetComponent<ControllablePhysicsComponent>();
	if (controllablePhysicsComponent) {
		controllablePhysicsComponent->ActivateBubbleBuff(eBubbleType::SKUNK);
		controllablePhysicsComponent->SetVelocity(NiPoint3(0.0f, 7.0f, 0.0f));
	}

	Game::entityManager->SerializeEntity(self);
	const auto casterId = caster->GetObjectID();
	self->AddCallbackTimer(2.0f, [self, casterId]() {
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
