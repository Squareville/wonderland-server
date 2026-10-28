#pragma once
#include "CppScripts.h"

class LupenFireknight : public CppScripts::Script {
	void OnStartup(Entity* self) override;
	void OnHit(Entity* self, Entity* attacker) override;

};
