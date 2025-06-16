#include "NpFelix.h"
#include "Entity.h"
#include "Character.h"
#include "eMissionState.h"
#include "MissionComponent.h"
#include "eTerminateType.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"
#include "Amf3.h"
#include <ranges>

void NpFelix::OnStartup(Entity* self) {
	// Already initialized for a given lot or is not a lot we need to set this up for
	if (m_TeleportArgs.contains(self->GetLOT()) || !m_SceneLotMap.contains(self->GetLOT())) return;
	auto& teleportArgs = m_TeleportArgs[self->GetLOT()];

	teleportArgs.Insert("strIdentifier", "FastTravel");
	teleportArgs.Insert("title", "%[UI_CHOICE_DESTINATION]");

	auto& choiceOptions = *teleportArgs.InsertArray("options");
	// insert the scene image into the map
	for (const auto& [secondLot, sceneId] : m_SceneLotMap) {
		// Skip this object
		if (secondLot == self->GetLOT()) continue;

		auto& sceneArgs = *choiceOptions.PushArray();
		const auto sceneIdStr = std::to_string(sceneId);
		sceneArgs.Insert("image", "textures/ui/zone_thumnails/np_scene" + sceneIdStr + ".dds");
		sceneArgs.Insert("caption", "%[UI_FELIX_CHOICE_SCENE" + sceneIdStr + "]");
		sceneArgs.Insert("identifier", "felix" + sceneIdStr + "spawn");
		sceneArgs.Insert("tooltipText", "%[UI_FELIX_CHOICE_SCENE" + sceneIdStr + "_HOVER]");
	}
}

void NpFelix::OnUse(Entity* self, Entity* player) {
	if (!player || !m_TeleportArgs.contains(self->GetLOT())) return;
	auto& teleportArgs = m_TeleportArgs[self->GetLOT()];
	teleportArgs.Insert("callbackClient", std::to_string(self->GetObjectID()));

	// check if the player has completed the mission
	auto missionComponent = player->GetComponent<MissionComponent>();
	if (!missionComponent) return;
	if (missionComponent->GetMissionState(m_MissionId) != eMissionState::COMPLETE) return;

	GameMessages::SendUIMessageServerToSingleClient(player, player->GetSystemAddress(), "QueueChoiceBox", teleportArgs);
}

void NpFelix::OnMessageBoxResponse(Entity* self, Entity* sender, int32_t button, const std::u16string& identifier, const std::u16string& userData) {
	if (!m_TeleportArgs.contains(self->GetLOT()) || !m_SceneLotMap.contains(self->GetLOT())) return;

	if (button != 1) {
		GameMessages::SendTerminateInteraction(sender->GetObjectID(), eTerminateType::FROM_INTERACTION, sender->GetObjectID());
	} else {
		const auto& teleportArgs = m_TeleportArgs.find(self->GetLOT())->second;
		const auto senderObjId = sender->GetObjectID();
		const auto senderSysAddr = sender->GetSystemAddress();
		const auto identifierAsStr = GeneralUtils::UTF16ToWTF8(identifier);

		// Bad arg if out of bounds
		GameMessages::SendSetStunned(senderObjId, eStateChangeType::PUSH, senderSysAddr, LWOOBJID_EMPTY,
			/* bCantAttack */ true,
			/* bCantEquip */ true,
			/* bCantInteract */ true,
			/* bCantJump */ true,
			/* bCantMove */ true,
			/* bCantTurn */ true,
			/* bCantUseItem */ true,
			/* bDontTerminateInteract */ true,
			/* bIgnoreImmunity */ true
		);

		auto time = RenderComponent::PlayAnimation(sender, "felix-teleport");
		sender->AddCallbackTimer(time, [sender, senderObjId, senderSysAddr, identifierAsStr]() {
			auto felixSpawn = Game::entityManager->GetEntitiesInGroup(identifierAsStr);
			if (felixSpawn.empty()) return;

			const auto felixEntity = felixSpawn[0];
			if (!felixEntity) return;

			const auto newPos = felixEntity->GetPosition();
			const auto newRot = felixEntity->GetRotation();
			GameMessages::SendTeleport(senderObjId, newPos, newRot, senderSysAddr, true);
			if (!sender) return;

			const auto time = RenderComponent::PlayAnimation(sender, "tube-resurrect");
			sender->AddCallbackTimer(time, [senderObjId, senderSysAddr]() {
				GameMessages::SendSetStunned(senderObjId, eStateChangeType::POP, senderSysAddr, LWOOBJID_EMPTY,
					/* bCantAttack */ true,
					/* bCantEquip */ true,
					/* bCantInteract */ true,
					/* bCantJump */ true,
					/* bCantMove */ true,
					/* bCantTurn */ true,
					/* bCantUseItem */ true,
					/* bDontTerminateInteract */ true,
					/* bIgnoreImmunity */ true
					);
				});
			});
	}

}

void NpFelix::OnChoiceBoxResponse(Entity* self, Entity* sender, int32_t button, const std::u16string& buttonIdentifier, const std::u16string& identifier) {
	if (button != -1) {
		GameMessages::SendDisplayMessageBox(sender->GetObjectID(), true, self->GetObjectID(), buttonIdentifier, 0, u"%[UI_FELIX_CHOICE_SCENE_CONFIRM]", u"", sender->GetSystemAddress());
	} else {
		GameMessages::SendTerminateInteraction(sender->GetObjectID(), eTerminateType::FROM_INTERACTION, self->GetObjectID());
	}
}
