#include "NpFelix.h"
#include "Entity.h"
#include "Character.h"
#include "eMissionState.h"
#include "MissionComponent.h"

void NpFelix::OnStartup(Entity* self) {
	self->SetVar(u"teleportAnim", m_TeleportAnim);
	self->SetVar(u"teleportString", m_TeleportString);

	teleportArgs.Reset();

	teleportArgs.Insert("callbackClient", std::to_string(self->GetObjectID()));
	teleportArgs.Insert("strIdentifier", "choiceDoor");
	teleportArgs.Insert("title", "%[UI_CHOICE_DESTINATION]");
	// find what our lot in the lotscenemap
	auto scene = m_SceneLotMap.find(self->GetLOT());
	if (scene == m_SceneLotMap.end()) return;

	// remove the scene we are currently in from the map
	m_SceneLotMap.erase(scene);

	auto& choiceOptions = *teleportArgs.InsertArray("options");
	// insert the scene image into the map

	for (const auto& [_, sceneId] : m_SceneLotMap) {
		auto image = m_SceneImageMap.find(sceneId);
		if (image == m_SceneImageMap.end()) continue;
		auto caption = m_SceneCaptionMap.find(sceneId);
		if (caption == m_SceneCaptionMap.end()) continue;
		auto tooltip = m_SceneTooltipMap.find(sceneId);
		if (tooltip == m_SceneTooltipMap.end()) continue;
		auto identifier = m_SceneIdentifierMap.find(sceneId);
		if (identifier == m_SceneIdentifierMap.end()) continue;

		auto& sceneArgs = *choiceOptions.PushArray();
		sceneArgs.Insert("image", image->second);
		sceneArgs.Insert("caption", caption->second);
		sceneArgs.Insert("identifier", 	identifier->second);
		sceneArgs.Insert("tooltipText", tooltip->second);
	}
}

void NpFelix::OnUse(Entity* self, Entity* player) {
	if (!player) return;

	// check if the player has completed the mission
	auto misstionComponent = player->GetComponent<MissionComponent>();
	if (!misstionComponent) return;
	if (misstionComponent->GetMissionState(m_MissionId) != eMissionState::COMPLETE) return;

	BaseOnUse(self, player);
}

void NpFelix::OnMessageBoxResponse(Entity* self, Entity* sender, int32_t button, const std::u16string& identifier, const std::u16string& userData) {

}

void NpFelix::OnChoiceBoxResponse(Entity* self, Entity* sender, int32_t button, const std::u16string& buttonIdentifier, const std::u16string& identifier) {

}

void NpFelix::OnTimerDone(Entity* self, std::string timerName) {

}

void NpFelix::OnFireEventServerSide(Entity* self, Entity* sender, std::string args, int32_t param1, int32_t param2, int32_t param3) {

}