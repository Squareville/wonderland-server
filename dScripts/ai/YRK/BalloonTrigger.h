#ifndef BALLOONTRIGGER_H
#define BALLOONTRIGGER_H

#include "CppScripts.h"

class BalloonTrigger : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
};

#endif  //!BALLOONTRIGGER_H
