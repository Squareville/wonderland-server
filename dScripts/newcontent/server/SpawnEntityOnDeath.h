#pragma once
#include "CppScripts.h"

class SpawnEntityOnDeath : public CppScripts::Script
{
public:
	SpawnEntityOnDeath(LOT lot = 4712) : m_SpawnLOT(lot) {};
	void OnDie(Entity* self, Entity* killer) override;
private:
	LOT m_SpawnLOT;
};
