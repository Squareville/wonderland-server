#pragma once
#include "CppScripts.h"

class SpawnEntityOnDeath : public CppScripts::Script
{
public:
	SpawnEntityOnDeath(LOT lot = 4712) : m_SpawnLOT(lot) {};
	SpawnEntityOnDeath(LOT lot, float timeout) : m_SpawnLOT(lot), m_Timeout(timeout) {};
	void OnDie(Entity* self, Entity* killer) override;
private:
	LOT m_SpawnLOT;
	float m_Timeout = 60.0f;
};
