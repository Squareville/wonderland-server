#include "NpMeteorCutsceneTrigger.h"
#include "Character.h"
#include "Entity.h"
#include "EntityManager.h"
#include "GameMessages.h"

namespace {
	constexpr uint32_t NpMeteorCutsceneFlag = 200;
}

void NpMeteorCutsceneTrigger::OnCollisionPhantom(Entity* self, Entity* target) {
	GameMessages::GetFlag flagMsg;
	flagMsg.target = target->GetObjectID();
	flagMsg.flagID = NpMeteorCutsceneFlag;
	if (!flagMsg.Send() || flagMsg.flag) return;

	// Wait so the cutscene is sure to have played on the client before setting the flag
	const auto playerID = target->GetObjectID();
	self->AddCallbackTimer(24.0f, [playerID]() {
		auto* const player = Game::entityManager->GetEntity(playerID);
		if (!player) return;

		auto* const character = player->GetCharacter();
		if (character) character->SetPlayerFlag(NpMeteorCutsceneFlag, true);
	});
}
