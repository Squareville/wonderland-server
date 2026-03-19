#include "TriggerGas.h"
#include "InventoryComponent.h"
#include "SkillComponent.h"
#include "Entity.h"
#include "Logger.h"

#include <ranges>

using Players = std::vector<LWOOBJID>;

void TriggerGas::OnStartup(Entity* self) {
	self->AddTimer(this->m_TimerName, this->m_Time);
}

void TriggerGas::OnCollisionPhantom(Entity* self, Entity* target) {
	if (!target || !target->IsPlayer()) return;
	Players players = self->GetVar<Players>(u"players");
	players.push_back(target->GetObjectID());
	self->SetVar(u"players", players);
}

void TriggerGas::OnOffCollisionPhantom(Entity* self, Entity* target) {
	if (!target || !target->IsPlayer()) return;
	Players players = self->GetVar<Players>(u"players");
	if (players.empty()) return;
	auto position = std::ranges::find(players, target->GetObjectID());
	if (position != players.end()) players.erase(position);
	self->SetVar(u"players", players);
}

void TriggerGas::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName != this->m_TimerName) return;
	Players players = self->GetVar<Players>(u"players");
	for (const auto playerID : players) {
		auto* const player = Game::entityManager->GetEntity(playerID);
		if (!player || player->GetIsDead()){
			auto position = std::ranges::find(players, playerID);
			if (position != players.end()) players.erase(position);
			continue;
		}
		auto* const inventoryComponent = player->GetComponent<InventoryComponent>();
		if (inventoryComponent) {
			if (!inventoryComponent->IsEquipped(this->m_MaelstromHelmet)) {
				auto* const skillComponent = self->GetComponent<SkillComponent>();
				if (skillComponent) {
					skillComponent->CastSkill(this->m_FogDamageSkill, player->GetObjectID());
				}
			}
		}
	}
	self->SetVar(u"players", players);
	self->AddTimer(this->m_TimerName, this->m_Time);
}

