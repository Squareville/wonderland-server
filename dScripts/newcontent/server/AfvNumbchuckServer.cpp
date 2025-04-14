#include "AfvNumbchuckServer.h"

#include "eAnimationFlags.h"
#include "RenderComponent.h"

void AfvNumbchuckServer::OnStartup(Entity* self) {
	self->SetProximityRadius(25.0f, "NumbChuckRadius");
	self->SetVar<uint32_t>(u"PlayerCount", 0);
}

void AfvNumbchuckServer::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
	if (name != "NumbChuckRadius") return;
	auto curPlayerCount = self->GetVar<uint32_t>(u"PlayerCount");
	LOG("%s %s %i", name.c_str(), status.c_str(), curPlayerCount);
	if (status == "ENTER") {
		self->SetVar<uint32_t>(u"PlayerCount", curPlayerCount + 1);

		if (curPlayerCount == 0) {
			GameMessages::SendChangeIdleFlags(self->GetObjectID(), eAnimationFlags::IDLE_ORGAN, eAnimationFlags::IDLE_NONE, UNASSIGNED_SYSTEM_ADDRESS);
			RenderComponent::PlayAnimation(self, "wake", 0.4f);
		}
	} else if (status == "LEAVE") {
		if (curPlayerCount != 0) self->SetVar<uint32_t>(u"PlayerCount", curPlayerCount - 1);

		if (curPlayerCount == 0) {
			GameMessages::SendChangeIdleFlags(self->GetObjectID(), eAnimationFlags::IDLE_NONE, eAnimationFlags::IDLE_ORGAN, UNASSIGNED_SYSTEM_ADDRESS);
			RenderComponent::PlayAnimation(self, "back2sleep", 0.4f);
		}
	}
}
