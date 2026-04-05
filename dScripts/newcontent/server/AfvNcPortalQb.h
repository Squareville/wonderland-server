#pragma once
#include "CppScripts.h"

class AfvNcPortalQb : public CppScripts::Script {
public:
	void OnQuickBuildComplete(Entity* self, Entity* target) override;
	void OnDie(Entity* self, Entity* killer) override;
};
