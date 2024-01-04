#pragma once
#include "CppScripts.h"

class SpawnSnowmanOnDeath : public CppScripts::Script
{
public:
	void OnQuickBuildComplete(Entity* self, Entity* target) override;
};
