// otherwise,
// SCENARIO: the player interacts with felix in scene 7

// a "choose your destination" menu appears (see lego club to NS/NT menu for example of usage),

// from here, the player can choose to go to scenes 1 through 6 - but not 7, since they're interacting with the felix in scene 7 already,

// the player chooses scene 3 - the script then stuns the player, plays effect 20158 with type "felix-teleport" on the player, then after enough time for that to complete,,

// looks for a group named felix3spawn, and teleports the player to the position/rotation of the object in that group,

// plays the "tube-resurrect" animation on the player, and unstuns them at the end (this whole sequence is mechanically very similar to the nexus tower paradox teleporters),


#ifndef NP_FELIX_H
#define NP_FELIX_H

#include "Amf3.h"
#include "CppScripts.h"

#include <map>

class NpFelix : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
	void OnUse(Entity* self, Entity* player) override;
	void OnMessageBoxResponse(Entity* self, Entity* sender, int32_t button, const std::u16string& identifier, const std::u16string& userData) override;
	void OnChoiceBoxResponse(Entity* self, Entity* sender, int32_t button, const std::u16string& buttonIdentifier, const std::u16string& identifier) override;
private:
	static constexpr uint32_t m_MissionId = 20250;
	std::map<LOT, int> m_SceneLotMap = {
		{ 20126, 1 },
		{ 20127, 2 },
		{ 20128, 3 },
		{ 20129, 4 },
		{ 20130, 5 },
		{ 20131, 6 },
		{ 20132, 7 } 
	};

	std::map<LOT, AMFArrayValue> m_TeleportArgs{};
};

#endif // NP_FELIX_H
