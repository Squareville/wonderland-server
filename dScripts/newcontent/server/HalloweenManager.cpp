#include "HalloweenManager.h"

#include "EntityInfo.h"
#include "Entity.h"
#include "EntityManager.h"
#include "GeneralUtils.h"

void HalloweenManager::OnStartup(Entity* self) {
	SpawnTheBossSmashable(self);
}

void HalloweenManager::SpawnTheBossSmashable(Entity* self) {

	auto choice = GeneralUtils::GenerateRandomNumber<uint32_t>(1,3);
	EntityInfo info{};
	info.spawnerID = self->GetObjectID();

	switch (choice) {
		case 1: {
			info.lot = m_Horseman;
			info.pos = m_SpawnLocationA;
			info.rot = m_SpawnRotationA;
			break;
		}
		case 2: {
			info.lot = m_Vampire;
			info.pos = m_SpawnLocationB;
			info.rot = m_SpawnRotationB;
			break;
		}
		case 3: 
		default: {
			info.lot = m_Mummy;
			info.pos = m_SpawnLocationA;
			info.rot = m_SpawnRotationA;
			break;
		}
	}

	auto spawnedEntity = Game::entityManager->CreateEntity(info, nullptr, self);
	Game::entityManager->ConstructEntity(spawnedEntity);
	spawnedEntity->AddDieCallback([this, self]() {
			HandleTheBossSmashableDeath(self);
		}
	);
}

void HalloweenManager::HandleTheBossSmashableDeath(Entity* self) {
	self->AddCallbackTimer(10, [this, self]() {
			SpawnTheBossSmashable(self);
		}
	);
}
