#include "SpawnEntityOnDeath.h"
#include "EntityInfo.h"
#include "Entity.h"
#include "EntityManager.h"

void SpawnEntityOnDeath::OnDie(Entity* self, Entity* killer) {
	EntityInfo info{};
	info.lot = m_SpawnLOT;
	info.pos = self->GetPosition();
	info.spawnerID = killer->GetObjectID();
	auto spawnedEntity = Game::entityManager->CreateEntity(info, nullptr, killer);
	Game::entityManager->ConstructEntity(spawnedEntity);
}
