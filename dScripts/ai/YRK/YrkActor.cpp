#include "YrkActor.h"
#include "Entity.h"
#include "Game.h"
#include "EntityManager.h"

void YrkActor::OnStartup(Entity* self) {
	Game::entityManager->GetZoneControlEntity()->OnObjectLoaded(self->GetObjectID(), self->GetLOT());
}
