#include "PicnicBotPicnicBot.h"

#include "RenderComponent.h"
#include "Zone.h"

void PicnicBotPicnicBot::OnArrived(Entity& self, const std::string& pathType, const uint32_t waypoint, const Path* const levelPath) {
	if (!levelPath || waypoint >= levelPath->pathWaypoints.size() || levelPath->pathWaypoints[waypoint].commands.empty()) return;
	if (levelPath->pathWaypoints[waypoint].commands[0].data == "picnicbot") {
		for (auto* const entity : Game::entityManager->GetEntitiesInGroup("picnicbot")) {
			if (entity) {
				RenderComponent::PlayAnimation(entity, "attack");
			}
		}
		self.Smash();
	}
}
