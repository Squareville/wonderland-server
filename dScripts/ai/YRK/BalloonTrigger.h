#ifndef __BALLOONTRIGGER__H__
#define __BALLOONTRIGGER__H__

#include "CppScripts.h"

class Entity;

class BalloonTrigger : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
};

#endif  //!__BALLOONTRIGGER__H__
