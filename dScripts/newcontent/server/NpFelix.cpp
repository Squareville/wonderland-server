#include "NpFelix.h"
#include "Chatacter.h"
#include "eMissionState.h"

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

	auto chatacter = player->GetCharacter();
	if (!chatacter) return;
	if (character->GetMissionState(m_MissionId) != eMissionState::COMPLETED) return;	

	BaseOnUse(self, player);
}

