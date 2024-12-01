#ifndef YRKNPCSTINK_H
#define YRKNPCSTINK_H

#include "CppScripts.h"

class YrkNpcStink : public CppScripts::Script {
public:	
	void OnStartup(Entity* self) override;
	void OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) override;
	void OnSkillEventFired(Entity* self, Entity* caster, const std::string& message) override;
	void OnTimerDone(Entity* self, std::string name) override;
private:
	const int32_t broomBotLot = 3247;
};

#endif  //!YRKNPCSTINK_H
