#include "ActPetInstance.h"

#include "Entity.h"
#include "Player.h"
#include "Amf3.h"
#include "GameMessages.h"

void ActPetInstance::OnUse(Entity* self, Entity* user) {
		// 	-- show a dialog box
		// local args = { 	{"bShow", true},
		// 				{"imageID", 3},
		// 				{"callbackClient", self},
		// 				{"text", strText},
		// 				{"strIdentifier", "Pet_Ranch_Start"} }
	GameMessages::SendDisplayMessageBox(user->GetObjectID(), true, self->GetObjectID(), u"Pet_Ranch_Start", 3, u"Enter Pet Ranch?", u"", user->GetSystemAddress());
}

void ActPetInstance::OnMessageBoxResponse(Entity* self, Entity* sender, int32_t button, const std::u16string& identifier, const std::u16string& userData) {
	Game::logger->Log("ActPetInstance", "%i %s", button, GeneralUtils::UTF16ToWTF8(identifier).c_str());
	if (button == 1 && identifier == u"Pet_Ranch_Start") {
		auto* player = dynamic_cast<Player*>(sender);
		if (!player) return;
		player->SendToZone(3201, GeneralUtils::GenerateRandomNumber<int32_t>(1, INT32_MAX));
	}
}
