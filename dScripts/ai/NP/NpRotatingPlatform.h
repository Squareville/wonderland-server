#ifndef NP_ROTATING_PLATFORM_H
#define NP_ROTATING_PLATFORM_H

#include "CppScripts.h"

class NpRotatingPlatform : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
	void OnTimerDone(Entity* self, std::string timerName)  override;
};

#endif // NP_ROTATING_PLATFORM_H
