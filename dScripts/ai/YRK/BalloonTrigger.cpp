#include "BalloonTrigger.h"
#include "Entity.h"

void BalloonTrigger::OnStartup(Entity* self) {
	self->SetProximityRadius(5.0f, "BalloonTrigger"); // this is probably used by the MPC to know when to move
}
