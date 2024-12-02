#include "LevelProgressionComponent.h"
#include "dZoneManager.h"
#include "WorldConfig.h"
#include "Character.h"
#include "ePlayerFlag.h"


namespace WonderlandCommands {
	void ToggleXP(Entity* entity, const SystemAddress& sysAddr, const std::string args) {
		auto levelComponent = entity->GetComponent<LevelProgressionComponent>();
		if (levelComponent != nullptr) {
			if (levelComponent->GetLevel() >= Game::zoneManager->GetWorldConfig()->levelCap) {
				auto character = entity->GetCharacter();
				character->SetPlayerFlag(ePlayerFlag::GIVE_USCORE_FROM_MISSIONS_AT_MAX_LEVEL, !character->GetPlayerFlag(ePlayerFlag::GIVE_USCORE_FROM_MISSIONS_AT_MAX_LEVEL));
				if (character->GetPlayerFlag(ePlayerFlag::GIVE_USCORE_FROM_MISSIONS_AT_MAX_LEVEL)) {
					GameMessages::SendSlashCommandFeedbackText(entity, u"You will now get coins and u-score as rewards.");
				} else { 
					GameMessages::SendSlashCommandFeedbackText(entity, u"You will now get only coins as rewards.");
				}
				return;
			}
			GameMessages::SendSlashCommandFeedbackText(entity, u"You must be at the max level to use this command.");
		}
	}
}