#include "YrkNpcOnTimer.h"
#include "MovementAIComponent.h"
#include "RenderComponent.h"

#include "Entity.h"

void YrkNpcOnTimer::OnStartup(Entity* self) {
	Game::entityManager->GetZoneControlEntity()->OnObjectLoaded(self->GetObjectID(), self->GetLOT());
	self->SetProximityRadius(20.0f, "WaveRadius");
}

void YrkNpcOnTimer::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
	if (entering->IsPlayer() && status == "ENTER" && name == "WaveRadius") {
		GameMessages::SendPlayFXEffect(self->GetObjectID(), 209, u"emote", "emote");
	}
}

void YrkNpcOnTimer::OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1, int32_t param2) {
	auto* const movementAiComponent = self->GetComponent<MovementAIComponent>();
	if (!movementAiComponent) return;
	if (name == "npc_panic") {
		movementAiComponent->SetMaxSpeed(5);
	} else if (name == "npc_idle") {
		movementAiComponent->SetMaxSpeed(0.5f);
	}
}
