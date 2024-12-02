#include "ActPetInstance.h"

#include "GameMessages.h"
#include "CharacterComponent.h"
#include "eTerminateType.h"

void ActPetInstance::OnUse(Entity* self, Entity* user) {
	GameMessages::SendDisplayMessageBox(
		user->GetObjectID(),
		true,
		self->GetObjectID(),
		u"Instance_Exit",
		1,
		u"ENTER_PET_RANCH",
		u"",
		user->GetSystemAddress()
	);
}

void ActPetInstance::OnMessageBoxResponse(Entity* self, Entity* sender, int32_t button, const std::u16string& identifier, const std::u16string& userData) {
	if (!sender->IsPlayer()) return;

	if (identifier == u"Instance_Exit" && button == 1) {
		const auto* const characterComponent = sender->GetComponent<CharacterComponent>();
		if (characterComponent) characterComponent->SendToZone(3201); // Pet Ranch
	}

	GameMessages::SendTerminateInteraction(sender->GetObjectID(), eTerminateType::FROM_INTERACTION, self->GetObjectID());
}



