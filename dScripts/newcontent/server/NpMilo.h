// Darkflame Universe
// Copyright 2025

#ifndef NPMILO_H
#define NPMILO_H

#include "CppScripts.h"

class NpMilo : public CppScripts::Script {
public:
	void OnDie(Entity* self, Entity* killer) override;
};

#endif //!NPMILO_H
