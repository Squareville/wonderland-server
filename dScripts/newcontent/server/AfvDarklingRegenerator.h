#ifndef AFVDARKLINGREGENERATOR_H
#define AFVDARKLINGREGENERATOR_H

#include "CppScripts.h"

class AfvDarklingRegenerator : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
	void OnTimerDone(Entity* self, std::string timerName) override;
};

#endif //!AFVDARKLINGREGENERATOR_H
