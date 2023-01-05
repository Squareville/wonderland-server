#ifndef __BALLOON__H__
#define __BALLOON__H__

#include "CppScripts.h"

class Entity;

class Balloon: public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
	void OnFireEventServerSide(Entity* self, Entity* sender, std::string args, int32_t param1, int32_t param2, int32_t param3) override;
	void OnTimerDone(Entity* self, std::string timerName) override;
	void OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1 = 0, int32_t param2 = 0) override;
	void OnWaypointReached(Entity* self, uint32_t waypointIndex) override;
private:
	void ResetStink(Entity* self);
	void SetStinkToMax(Entity* self);
	void CheckForLaunch(Entity* self);
	void LaunchBalloon(Entity* self);
	void UpdateStinkAtSwitch(Entity* self);
	void SetRecheckTimer(Entity* self);
	void GiveCreditForLaunching(Entity* self);
	void UpdateAnimation(Entity* self, uint32_t stinkAmount);
	const uint32_t sufficientStink = 2;
	const uint32_t skunkStinkSkill = 33;
	const uint32_t imitationSkunkSkill = 35;
	const float recheckTime = 0.5;
};

#endif  //!__BALLOON__H__
