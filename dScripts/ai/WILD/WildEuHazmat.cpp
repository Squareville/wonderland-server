#include "WildEuHazmat.h"
#include "DestroyableComponent.h"

void WildEuHazmat::OnStartup(Entity* self) {
	auto* const destroyableComponent = self->GetComponent<DestroyableComponent>();
	if (!destroyableComponent) return;

	destroyableComponent->SetFaction(5);
}
