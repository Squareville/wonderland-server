#include "AfvBell.h"

void AfvBell::OnUse(Entity* self, Entity* user) {
	GameMessages::SendPlayNDAudioEmitter(self, UNASSIGNED_SYSTEM_ADDRESS, "{7dab93a9-6ee5-4ba0-8c51-067477348a02}");
}
