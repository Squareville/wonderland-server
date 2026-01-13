#include "EnemySkunk.h"

#include "ControllablePhysicsComponent.h"
#include "MovementAIComponent.h"
#include "SkillComponent.h"
#include "RenderComponent.h"

// important note - the original lua version of this script is long gone, but what it did has been inferred and guesstimated from the zp shooting gallery skunk scripts (l_yrk_sg_skunk_target.lua, l_yrk_sg_skunk_target_baby.lua)

void EnemySkunk::OnStartup(Entity* self) {
	self->SetProximityRadius(3.0f, "StinkyPlayer");
}

void EnemySkunk::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
	if (!entering || status != "ENTER" || name != "StinkyPlayer") return;

	// this whole "random chance to stink player via prox radius" thing is kinda guesstimated cause we don't wanna deal with combat AI + pathing, and don't know exactly how it originally worked anyway
	if (GeneralUtils::GenerateRandomNumber<uint32_t>(0, 100) <= 30) {
		auto* skillComponent = self->GetComponent<SkillComponent>();
		if (!skillComponent) return;

		skillComponent->CastSkill(772, entering->GetObjectID());
	}
}

void EnemySkunk::OnSkillEventFired(Entity* self, Entity* caster, const std::string& message) {
	// waterspray is NS water sprayer, soapspray is NT janitor outfit (just cause it feels like players "should" be able to use it here)
	if (message != "waterspray" && message != "soapspray") return;

	auto* const movementAiComponent = self->GetComponent<MovementAIComponent>();
	if (movementAiComponent) movementAiComponent->Stop();

	auto* const controllablePhysicsComponent = self->GetComponent<ControllablePhysicsComponent>();
	if (controllablePhysicsComponent) {
		controllablePhysicsComponent->ActivateBubbleBuff(eBubbleType::SKUNK);
		// going by the zp shooting gallery skunk scripts, the live implementation would have been to:
		// - immediately add 5 to the skunk's current position to "help the skunk break free of gravity"
		// - start a timer firing every 0.1 seconds, adding 15 to its current y velocity each time to continually counteract gravity
		// however, as DLU doesn't do the whole "gravity" thing, simply setting the skunk's y velocity to 15 once works for now
		// if anyone ever adds gravity simulation, you will need to change this implementation as described above
		controllablePhysicsComponent->SetVelocity(NiPoint3(0.0f, 15.0f, 0.0f));
		RenderComponent::PlayAnimation(self, "howl");
	}

	Game::entityManager->SerializeEntity(self);
	const auto casterId = caster->GetObjectID();
	// it seems that live zp skunks died (silently) upon reaching a certain height; 300 or above in the shooting gallery for sure
	// we opt to instead destroy them on a brief timer, tuned to "whatever felt good"
	// we also give player the kill credit via the smash itself (and they drop loot)
	// while in live, the script would have manually progressed "kill skunk" missions/achievements immediately upon being bubbled, and the kill would have been entirely silent
	self->AddCallbackTimer(1.7f, [self, casterId]() {
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
