#pragma once
#include "CppScripts.h"

class QbEnemyStunner : public CppScripts::Script
{
public:
	void OnQuickBuildComplete(Entity* self, Entity* target) override;
	void OnTimerDone(Entity* self, std::string timerName) override;
};
