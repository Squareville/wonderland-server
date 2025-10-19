#include "XMarksTheSpotChest.h"

#include "DestroyableComponent.h"
#include "Game.h"
#include "GameMessages.h"
#include "Loot.h"
#include "TeamManager.h"

// modified from a scrapped rank 1 buccaneer script and the numbers and drops have been adjusted

void
XMarksTheSpotChest::OnStartup(Entity* self) {
	self->AddTimer("SpawnGoodies", 1);
}

void XMarksTheSpotChest::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName == "SpawnGoodies") {
		const auto spawnPosition = self->GetPosition();
		const auto source = self->GetObjectID();
		const std::unordered_map<LOT, int32_t> result(
			{
				{11916, GeneralUtils::GenerateRandomNumber<uint32_t>(2, 4)}, // life 3 points
				{11913, GeneralUtils::GenerateRandomNumber<uint32_t>(2, 4)}, // armor 3 points
				{11910, GeneralUtils::GenerateRandomNumber<uint32_t>(2, 4)} // imagination 3 points
			}
		);

		auto* entity = self->GetParentEntity();

		for (const auto& [item, count] : result) {
			GameMessages::DropClientLoot lootMsg{};
			lootMsg.target = entity->GetObjectID();
			lootMsg.ownerID = entity->GetObjectID();
			lootMsg.sourceID = source;
			lootMsg.item = item;
			lootMsg.count = 1;
			lootMsg.spawnPos = spawnPosition;
			for (int i = 0; i < count; ++i) {
				Loot::DropItem(*entity, lootMsg, true);
			}
		}

		self->AddTimer("Die", 1);
	} else if (timerName == "Die") {
		auto* destComp = self->GetComponent<DestroyableComponent>();
		if (destComp) {
			destComp->Smash(self->GetObjectID(), eKillType::SILENT);
		}
	}
}
