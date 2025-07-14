#include "NpMime.h"

#include "EntityManager.h"
#include "RenderComponent.h"
#include "MovementAIComponent.h"

void NpMime::OnWaypointReached(Entity* self, uint32_t waypointIndex) {
	if (waypointIndex == 0) {
		auto movementAI = self->GetComponent<MovementAIComponent>();
		if (!movementAI) return;
		movementAI->Pause(14.3f);
		self->SetPosition(m_JugglePosition);
		self->SetRotation(m_JuggleRotation);

		for (auto* juggler : Game::entityManager->GetEntitiesInGroup("Jugglers")) {
			if (!juggler) continue;
			RenderComponent::PlayAnimation(juggler, "cast", 0.4f);
		}
		RenderComponent::PlayAnimation(self, "juggle", 0.4f);
	} else if (waypointIndex == 3) {
		auto movementAI = self->GetComponent<MovementAIComponent>();
		if (!movementAI) return;
		movementAI->Pause(14.1f);
		RenderComponent::PlayAnimation(self, "box", 0.4f);
	} else if (waypointIndex == 4) {
		auto movementAI = self->GetComponent<MovementAIComponent>();
		if (!movementAI) return;
		movementAI->Pause(17.5f);
		RenderComponent::PlayAnimation(self, "lasso", 0.4f);
	}
}

void NpMime::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName == "juggle") {
		auto movementAI = self->GetComponent<MovementAIComponent>();
		if (!movementAI) return;
		movementAI->Resume();
	}
}
