#ifndef __SPOUT__H__
#define __SPOUT__H__

#include "CppScripts.h"

enum class SkunkEventState : int32_t;

class Spout : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
	void OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) override;
	void OnTimerDone(Entity* self, std::string timerName) override;
	void OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1 = 0, int32_t param2 = 0) override;
private:
	bool GetEnableState(Entity* self);
	void SetEnableState(Entity* self, SkunkEventState state);
	void CleanPlayer(Entity* self, Entity* entering);
	bool ArePlayersInProximity(Entity* self);
};

#endif  //!__SPOUT__H__
