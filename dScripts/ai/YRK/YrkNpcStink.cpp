#include "YrkNpcStink.h"

#include "ControllablePhysicsComponent.h"
#include "MissionComponent.h"
#include "eMissionTaskType.h"

#include "Loot.h"

void YrkNpcStink::OnStartup(Entity* self) {
	self->SetProximityRadius(2.0f, "bubbleradius");
}

void YrkNpcStink::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
	if (status == "ENTER" && entering->IsPlayer()) {
		const auto* const controllablePhysicsComponent = entering->GetComponent<ControllablePhysicsComponent>();
		if (!controllablePhysicsComponent) return;
		if (controllablePhysicsComponent->GetIsInBubble()) {
			Loot::DropCoins(entering, self->GetObjectID(), 1, 1);
			Game::entityManager->GetZoneControlEntity()->NotifyObject(entering, "stink_cloud_cleaned_by_player", self->GetVar<int32_t>(u"StinkCloudNum"));
			self->Smash(LWOOBJID_EMPTY, eKillType::VIOLENT);
		}
	}
}

void YrkNpcStink::OnSkillEventFired(Entity* self, Entity* caster, const std::string& message) {
	if (message != "waterspray" && message != "skunkstink") return;

	if (caster->GetLOT() == broomBotLot) {
		self->AddTimer("RemoveSelf", 4.0f);
		auto* const player = GetEntityByName(self, u"playerBuilder");
		if (player) {
			Game::entityManager->GetZoneControlEntity()->NotifyObject(player, "stink_cloud_cleaned_by_player", self->GetVar<int32_t>(u"StinkCloudNum"));
		} else {
			Game::entityManager->GetZoneControlEntity()->NotifyObject(caster, "stink_cloud_cleaned_by_broombot", self->GetVar<int32_t>(u"StinkCloudNum"));
		}
	} else {
		Loot::DropCoins(caster, self->GetObjectID(), 1, 1);
		auto* missionComponent = caster->GetComponent<MissionComponent>();
		if (missionComponent) {
			missionComponent->Progress(eMissionTaskType::SMASH, self->GetLOT());
		}
		Game::entityManager->GetZoneControlEntity()->NotifyObject(caster, "stink_cloud_cleaned_by_player", self->GetVar<int32_t>(u"StinkCloudNum"));
		self->Smash(LWOOBJID_EMPTY, eKillType::VIOLENT);
	}
}

void YrkNpcStink::OnTimerDone(Entity* self, std::string name) {
	if (name == "RemoveSelf") {
		self->Smash();
	}
}
