#include "ZonePlayer.h"
#include "GameMessages.h"
#include "CharacterComponent.h"
#include "Entity.h"
#include "eStateChangeType.h"

void ZonePlayer::OnUse(Entity* self, Entity* user) {
	GameMessages::SendSetStunned(user->GetObjectID(), eStateChangeType::PUSH, user->GetSystemAddress(), self->GetObjectID(),
	true /* bCantAttack */,
	false /* bCantEquip */,
	true /* bCantInteract */,
	false /* bCantJump */,
	true /* bCantMove */);
	
	GameMessages::SendNotifyClientObject(user->GetObjectID(), u"ZonePlayerFromServer", 0, 0, LWOOBJID_EMPTY, "", user->GetSystemAddress());
	auto* const characterComponent = user->GetComponent<CharacterComponent>();
	auto* const character = user->GetCharacter();
	if (characterComponent && character && self->HasVar(u"zoneID") && self->HasVar(u"spawnPoint")) {
		character->SetTargetScene(self->GetVarAs<std::string>(u"spawnPoint"));
		characterComponent->SendToZone(self->GetVarAs<LWOMAPID>(u"zoneID"));
	}
}
