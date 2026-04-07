#include "AfvNcPortalQb.h"
#include "Entity.h"
#include "EntityInfo.h"
#include "EntityManager.h"

void AfvNcPortalQb::OnQuickBuildComplete(Entity* self, Entity* target) {
	EntityInfo info{};
	info.lot = 20400;
	info.pos = self->GetPosition();
	info.pos.SetX(info.pos.GetX() + 4.0f);
	info.rot = self->GetRotation();
	info.spawnerID = self->GetObjectID();

	auto* child = Game::entityManager->CreateEntity(info);
	Game::entityManager->ConstructEntity(child);
	self->SetVar(u"Child", child->GetObjectID());

	for (auto* crystal : Game::entityManager->GetEntitiesInGroup("MaelstromCrystals")) {
		crystal->Smash(self->GetObjectID());
	}
}

void AfvNcPortalQb::OnDie(Entity* self, Entity* killer) {
	auto* child = Game::entityManager->GetEntity(self->GetVar<LWOOBJID>(u"Child"));
	if (child != nullptr) {
		child->Smash(LWOOBJID_EMPTY, eKillType::SILENT);
	}
}
