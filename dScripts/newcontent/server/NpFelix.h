// otherwise,
// SCENARIO: the player interacts with felix in scene 7

// a "choose your destination" menu appears (see lego club to NS/NT menu for example of usage),

// from here, the player can choose to go to scenes 1 through 6 - but not 7, since they're interacting with the felix in scene 7 already,

// the player chooses scene 3 - the script then stuns the player, plays effect 20158 with type "felix-teleport" on the player, then after enough time for that to complete,,

// looks for a group named felix3spawn, and teleports the player to the position/rotation of the object in that group,

// plays the "tube-resurrect" animation on the player, and unstuns them at the end (this whole sequence is mechanically very similar to the nexus tower paradox teleporters),


#ifndef NP_FELIX_H
#define NP_FELIX_H
#include "CppScripts.h"
#include "Entity.h"
#include <map>
#include "BaseConsoleTeleportServer.h"
#include "Amf3.h"


class NpFelix : public CppScripts::Script, BaseConsoleTeleportServer {
public:
	void OnStartup(Entity* self) override;
	void OnUse(Entity* self, Entity* player) override;
	void OnMessageBoxResponse(Entity* self, Entity* sender, int32_t button, const std::u16string& identifier, const std::u16string& userData) override;
	void OnChoiceBoxResponse(Entity* self, Entity* sender, int32_t button, const std::u16string& buttonIdentifier, const std::u16string& identifier) override;
	void OnTimerDone(Entity* self, std::string timerName) override;
	void OnFireEventServerSide(Entity* self, Entity* sender, std::string args, int32_t param1, int32_t param2, int32_t param3) override;
private:
	AMFArrayValue teleportArgs{};
	std::u16string m_TeleportAnim = u"felix-teleport";
	std::u16string m_TeleportAnimEnd = u"tube-resurrect";
	std::u16string m_TeleportString = u"FELIX_TELEPORT_WINDOW_TITLE";
	
	const uint32_t m_MissionId = 20228;
	std::map<LOT, int> m_SceneLotMap = {
		{ 20126, 1 },
		{ 20127, 2 },
		{ 20128, 3 },
		{ 20129, 4 },
		{ 20130, 5 },
		{ 20131, 6 },
		{ 20132, 7 } 
	};
	std::map<int, std::string> m_SceneImageMap = {
		{ 1, "textures/ui/zone_thumbnails/np_scene1.dds" },
		{ 2, "textures/ui/zone_thumbnails/np_scene2.dds" },
		{ 3, "textures/ui/zone_thumbnails/np_scene3.dds" },
		{ 4, "textures/ui/zone_thumbnails/np_scene4.dds" },
		{ 5, "textures/ui/zone_thumbnails/np_scene5.dds" },
		{ 6, "textures/ui/zone_thumbnails/np_scene6.dds" },
		{ 7, "textures/ui/zone_thumbnails/np_scene7.dds" }
	};
	std::map<int, std::string> m_SceneIdentifierMap = {
		{ 1, "felix1spawn" },
		{ 2, "felix2spawn" },
		{ 3, "felix3spawn" },
		{ 4, "felix4spawn" },
		{ 5, "felix5spawn" },
		{ 6, "felix6spawn" },
		{ 7, "felix7spawn" }
	};
	// captions
	std::map<int, std::string> m_SceneCaptionMap = {
		{ 1, "%[UI_FELIX_CHOICE_SCENE1]" },
		{ 2, "%[UI_FELIX_CHOICE_SCENE2]" },
		{ 3, "%[UI_FELIX_CHOICE_SCENE3]" },
		{ 4, "%[UI_FELIX_CHOICE_SCENE4]" },
		{ 5, "%[UI_FELIX_CHOICE_SCENE5]" },
		{ 6, "%[UI_FELIX_CHOICE_SCENE6]" },
		{ 7, "%[UI_FELIX_CHOICE_SCENE7]" }
	};
	// tooltips
	std::map<int, std::string> m_SceneTooltipMap = {
		{ 1, "%[UI_FELIX_CHOICE_SCENE1_HOVER]" },
		{ 2, "%[UI_FELIX_CHOICE_SCENE2_HOVER]" },
		{ 3, "%[UI_FELIX_CHOICE_SCENE3_HOVER]" },
		{ 4, "%[UI_FELIX_CHOICE_SCENE4_HOVER]" },
		{ 5, "%[UI_FELIX_CHOICE_SCENE5_HOVER]" },
		{ 6, "%[UI_FELIX_CHOICE_SCENE6_HOVER]" },
		{ 7, "%[UI_FELIX_CHOICE_SCENE7_HOVER]" }
	};
};

#endif // NP_FELIX_H
