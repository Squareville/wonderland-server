#include "SpawnSnowmanOnDeath.h"
#include "GeneralUtils.h"
#include "EntityInfo.h"
#include "Entity.h"
#include "EntityManager.h"

void SpawnSnowmanOnDeath::OnRebuildComplete(Entity* self, Entity* target) {
	// becuase some snowmen are high up and it would be stupid to spawn on them
	if (self->GetPosition().y > 550) return;
	auto chance = GeneralUtils::GenerateRandomNumber<uint32_t>(1, 100);
	if (chance <= 50){
		self->AddCallbackTimer(1.0f, [=]() {
			EntityInfo info{};
			info.lot = 20095;
			info.pos = self->GetPosition();
			info.spawnerID = self->GetSpawnerID();
			auto snowboi = Game::entityManager->CreateEntity(info, nullptr);
			Game::entityManager->ConstructEntity(snowboi);
			self->Smash(LWOOBJID_EMPTY, eKillType::SILENT);
		});
	}
}
