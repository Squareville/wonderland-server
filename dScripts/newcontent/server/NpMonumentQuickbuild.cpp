#include "NpMonumentQuickbuild.h"
#include "Character.h"
#include "Entity.h"
#include "GameMessages.h"

namespace {
	constexpr uint32_t NpMonumentQuickbuildFlag = 202;
}

void NpMonumentQuickbuild::OnQuickBuildComplete(Entity* self, Entity* target) {
	auto* const character = target->GetCharacter();
	if (character) character->SetPlayerFlag(NpMonumentQuickbuildFlag, true);

	GameMessages::SendNotifyClientObject(self->GetObjectID(), u"MonumentBuilt", 0, 0, LWOOBJID_EMPTY, "", target->GetSystemAddress());
}
