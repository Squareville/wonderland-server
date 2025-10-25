// Darkflame Universe
// Copyright 2025

#ifndef AFVBELL_H
#define AFVBELL_H

#include "CppScripts.h"

class AfvBell : public CppScripts::Script {
public:
	void OnUse(Entity* self, Entity* user) override;
};

#endif //!AFVBELL_H
