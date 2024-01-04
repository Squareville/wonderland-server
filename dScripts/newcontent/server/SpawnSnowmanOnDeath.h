#pragma once
#include "CppScripts.h"

class SpawnSnowmanOnDeath : public CppScripts::Script
{
public:
	void OnRebuildComplete(Entity* self, Entity* target) override;
};
