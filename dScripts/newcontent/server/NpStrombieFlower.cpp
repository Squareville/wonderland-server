#include "NpStrombieFlower.h"
#include "RenderComponent.h"
#include "EntityInfo.h"
#include "EntityManager.h"

void NpStrombieFlower::OnStartup(Entity* self) {
	self->SetProximityRadius(15, "strombie");
}

void NpStrombieFlower::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName == "hatchTime") {
		auto* renderComponent = self->GetComponent<RenderComponent>();

		if (renderComponent != nullptr) {
			renderComponent->PlayEffect(20175, u"spawnstrombie", "BurstFX1");
		}

		EntityInfo info{};
		info.lot = 20167;
		info.pos = self->GetPosition();
		info.rot = self->GetRotation();
		info.spawnerID = self->GetObjectID();

		auto* spawnedEntity = Game::entityManager->CreateEntity(info);

		if (spawnedEntity == nullptr) {
			return;
		}

		Game::entityManager->ConstructEntity(spawnedEntity);

		spawnedEntity->AddCallbackTimer(60, [spawnedEntity]() {
			spawnedEntity->Smash(spawnedEntity->GetObjectID());
			});

		self->Smash(self->GetObjectID());
	}
}

void NpStrombieFlower::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
	if (entering->IsPlayer() && name == "strombie" && status == "ENTER" && !self->GetVar<bool>(u"hatching")) {
		StartHatching(*self);
	}
}

void NpStrombieFlower::OnHit(Entity* self, Entity* attacker) {
	if (!self->GetVar<bool>(u"hatching")) {
		StartHatching(*self);
	}
}

void NpStrombieFlower::StartHatching(Entity& self) {
	self.SetVar(u"hatching", true);

	auto* renderComponent = self.GetComponent<RenderComponent>();

	if (renderComponent != nullptr) {
		renderComponent->PlayEffect(20174, u"approach", "WakeUpFX1");
	}

	self.AddTimer("hatchTime", 2);
}
