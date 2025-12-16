#include "SpawnMaelstromNinjaOnDeath.h"

#include "GeneralUtils.h"
#include "EntityInfo.h"
#include "Entity.h"
#include "EntityManager.h"

LOT RollLOT() {
	LOT chosenLOT{ LOT_NULL };
	auto chance = GeneralUtils::GenerateRandomNumber<uint32_t>(1, 2400);
	if (chance <= 400) {
		chosenLOT = 20154;
	} else if (chance <= 600) {
		chosenLOT = 20155;
	} else if (chance >= 2399) {
		chosenLOT = 20156;
	}
	
	return chosenLOT;
}

void SpawnMaelstromNinjaOnDeath::OnDie(Entity* self, Entity* killer) {
	const auto chosenLot = RollLOT();
	if (chosenLot == LOT_NULL) return;

	EntityInfo info{};
	info.lot = chosenLot;
	info.pos = self->GetPosition();
	info.spawnerID = killer->GetObjectID();
	Game::entityManager->ConstructEntity(Game::entityManager->CreateEntity(info, nullptr, killer));
}
