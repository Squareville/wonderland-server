#include "NpMime.h"

#include "EntityManager.h"
#include "RenderComponent.h"
#include "MovementAIComponent.h"

void NpMime::OnWaypointReached(Entity* self, uint32_t waypointIndex) {
	if (waypointIndex == 0) {
		auto movementAI = self->GetComponent<MovementAIComponent>();
		if (!movementAI) return;
		movementAI->Pause();
		self->AddTimer("ResumeMovement", 14.3f);
		self->SetPosition(m_JugglePosition);
		self->SetRotation(m_JuggleRotation);

		for (auto& juggler : Game::entityManager->GetEntitiesInGroup("jugglers")) {
			if (!juggler) continue;
			auto renderComponent = juggler->GetComponent<RenderComponent>();
			if (!renderComponent) continue;
			renderComponent->PlayAnimation(juggler, "cast");
		}
		auto renderComponent = self->GetComponent<RenderComponent>();
		if (!renderComponent) return;
		renderComponent->PlayAnimation(self, "juggle");
	} else if (waypointIndex == 3) {
		auto movementAI = self->GetComponent<MovementAIComponent>();
		if (!movementAI) return;
		movementAI->Pause();
		self->AddTimer("ResumeMovement", 14.1f);
		auto renderComponent = self->GetComponent<RenderComponent>();
		if (!renderComponent) return;
		renderComponent->PlayAnimation(self, "box");
	} else if (waypointIndex == 4) {
		auto movementAI = self->GetComponent<MovementAIComponent>();
		if (!movementAI) return;
		movementAI->Pause();
		self->AddTimer("ResumeMovement", 17.5f);
		auto renderComponent = self->GetComponent<RenderComponent>();
		if (!renderComponent) return;
		renderComponent->PlayAnimation(self, "lasso");
	}
}

void NpMime::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName == "juggle") {
		auto movementAI = self->GetComponent<MovementAIComponent>();
		if (!movementAI) return;
		movementAI->Resume();
	}
}
