#include "MazeTroll.h"

#include "Entity.h"
#include "RenderComponent.h"

void DrinkWater(Entity* self) {
	RenderComponent::PlayAnimation(self, "drink");
}

void FaceShooter(Entity* self) {
	auto shooter = self->GetVar<LWOOBJID>(u"shooter");
	// face the shooter
}

void onSquirtWithWatergun(Entity* self, Entity* squirter) {
	self->SetVar<LWOOBJID>(u"shooter", squirter->GetObjectID());
	FaceShooter(self);
	DrinkWater(self);
}
