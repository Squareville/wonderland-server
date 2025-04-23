#include "NpMilo.h"

#include "EntityInfo.h"

void NpMilo::OnDie(Entity* self, Entity* killer) {
	EntityInfo info{};
	info.lot = 20170;
	info.pos = self->GetPosition();
	info.rot = self->GetRotation();
	info.spawnerID = self->GetObjectID();
	Game::entityManager->ConstructEntity(Game::entityManager->CreateEntity(info));
}
