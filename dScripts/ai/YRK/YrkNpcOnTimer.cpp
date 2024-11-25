#include "YrkNpcOnTimer.h"
#include "MovementAIComponent.h"

void YrkNpcOnTimer::OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1, int32_t param2) {
	auto* const movementAiComponent = self->GetComponent<MovementAIComponent>();
	if (!movementAiComponent) return;
	if (name == "npc_panic") {
		movementAiComponent->SetMaxSpeed(5);
	} else if (name == "npc_idle") {
		movementAiComponent->SetMaxSpeed(0.5f);
	}
}
