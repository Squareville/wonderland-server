#include "BalloonTrigger.h"

#include "Entity.h"

void BalloonTrigger::OnStartup(Entity* self) {
	self->SetProximityRadius(5.0f, "balloon_radius");
}
