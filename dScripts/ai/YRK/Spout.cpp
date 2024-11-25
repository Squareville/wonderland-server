#include "Spout.h"
#include "Entity.h"
#include "Game.h"
#include "EntityManager.h"

void Spout::OnStartup(Entity* self) {
	Game::entityManager->GetZoneControlEntity()->OnObjectLoaded(self->GetObjectID(), self->GetLOT());
	self->SetVar<bool>(u"PlayerOnMe", false);
	self->SetProximityRadius(3.0f, "Spout");
	self->SetVar<bool>(u"SpoutEnabled", true);
}

void Spout::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {

}

void Spout::OnTimerDone(Entity* self, std::string name) {

}

void Spout::OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1, int32_t param2) {

}
