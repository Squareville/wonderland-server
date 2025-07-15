// Darkflame Universe
// Copyright 2025

#ifndef MECHEUBROOMBOT_H
#define MECHEUBROOMBOT_H

#include "CppScripts.h"

class MechEuBroombot : public CppScripts::Script {
public:
	void OnUse(Entity* self, Entity* other) override;
	void OnUpdate(Entity* self) override;
	void OnQuickBuildComplete(Entity* self, Entity* builder) override;
};

#endif //!MECHEUBROOMBOT_H
