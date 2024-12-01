#include "HazmatTruck.h"
#include "Entity.h"
#include "Game.h"
#include "EntityManager.h"

void HazmatTruck::OnStartup(Entity* self) {
	Game::entityManager->GetZoneControlEntity()->OnObjectLoaded(self->GetObjectID(), self->GetLOT());
}
