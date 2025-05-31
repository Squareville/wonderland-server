#include "NpPicnicBot.h"

#include "RenderComponent.h"

void NpPicnicBot::OnStartup(Entity* self) {
	//create a proximity collider for the picnic bot
	self->SetProximityRadius(4.0f, "PicnicBotProximity");
}

void NpPicnicBot::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
	if (name == "PicnicBotProximity" && status == "ENTER") {
		if (entering->GetLOT() == m_EnemyLot) {
			// Kill the enemy LOT
			entering->Smash();
			//get render component
			auto renderComponent = entering->GetComponent<RenderComponent>();
			if (!renderComponent) return;
			// Play attack animation on self
			renderComponent->PlayAnimation(self, "attack");
		}
	}
}
