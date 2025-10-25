// Darkflame Universe
// Copyright 2025

#ifndef ZONEPLAYER_H
#define ZONEPLAYER_H

#include "CppScripts.h"

class ZonePlayer : public CppScripts::Script {
public:
	void OnUse(Entity* self, Entity* user);
};

#endif //!ZONEPLAYER_H
