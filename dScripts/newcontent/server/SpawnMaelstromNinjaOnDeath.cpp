#include "SpawnMaelstromNinjaOnDeath.h"

#include "GeneralUtils.h"
#include "EntityInfo.h"
#include "Entity.h"
#include "EntityManager.h"

LOT RollLOT() {
	LOT chosenLOT{};
	auto chance = GeneralUtils::GenerateRandomNumber<uint32_t>(1, 50);
	if (chance <= 35) {
		chosenLOT = 20021;
	} else if (chance <= 49) {
		chosenLOT = 20022;
	} else if (chance == 50) {
		chosenLOT = 20051;
	}
	
	return chosenLOT;
}

void SpawnMaelstromNinjaOnDeath::OnDie(Entity* self, Entity* killer) {
	LOG("ROLLING");
	EntityInfo info{};
	info.lot = RollLOT();
	info.pos = self->GetPosition();
	info.spawnerID = killer->GetObjectID();
	Game::entityManager->ConstructEntity(Game::entityManager->CreateEntity(info, nullptr, killer));
}
