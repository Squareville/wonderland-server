#include "HazmatMissionGiver.h"

#include "DestroyableComponent.h"
#include "Entity.h"

void HazmatMissionGiver::OnStartup(Entity* self) {
	Game::logger->Log("HazmatMissionGiver", "pos is %f %f %f", self->GetPosition().x, self->GetPosition().y, self->GetPosition().z);
	auto* destroyableComponent = self->GetComponent<DestroyableComponent>();
	if (destroyableComponent) destroyableComponent->SetFaction(5);
}
