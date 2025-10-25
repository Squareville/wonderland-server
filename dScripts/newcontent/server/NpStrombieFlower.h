// Darkflame Universe
// Copyright 2025

#ifndef NPSTROMBIEFLOWER_H
#define NPSTROMBIEFLOWER_H

#include "CppScripts.h"

class NpStrombieFlower : public CppScripts::Script {
public:
	void OnStartup(Entity* self);
	void OnTimerDone(Entity* self, std::string timerName);
	void OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status);
	void OnHit(Entity* self, Entity* attacker);
	void StartHatching(Entity& self);
};

#endif //!NPSTROMBIEFLOWER_H
