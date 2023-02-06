#ifndef __HAZMATTRUCK__H__
#define __HAZMATTRUCK__H__

#include "CppScripts.h"

class HazmatTruck : public CppScripts::Script {
public:
	void OnRebuildNotifyState(Entity* self, eRebuildState state) override;
	void OnTimerDone(Entity* self, std::string timerName) override;
};

#endif  //!__HAZMATTRUCK__H__
