#include "HazmatMissionGiver.h"
#include "DestroyableComponent.h"
#include "Entity.h"

void HazmatMissionGiver::OnStartup(Entity* self) {
	auto* const destroyableComponent = self->GetComponent<DestroyableComponent>();
	if (!destroyableComponent) return;

	destroyableComponent->SetFaction(5);
}
