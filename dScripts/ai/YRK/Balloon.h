#ifndef BALLOON_H
#define BALLOON_H

#include "CppScripts.h"

class Balloon : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
	void OnFireEventServerSide(Entity* self, Entity* sender, std::string args, int32_t param1, int32_t param2, int32_t param3) override;
	void OnTimerDone(Entity* self, std::string name) override;
	void OnPlayerLoaded(Entity* self, Entity* player) override;
private:
	void SetStinkToMax(Entity& self);
	void CheckForLaunch(Entity& self);
	void GiveCreditForLaunching(Entity& self);
	void UpdateAnimation(Entity& self);
	void UpdateStinkAtSwitch(Entity& self);
	void LaunchBalloon(Entity& self);
	float m_SoftUpdateTimer = 2.0f;
	int32_t m_SufficientStink = 2;
	int32_t m_SkunkStinkSkill = 32;
	int32_t m_HeldItemSkunkStinkSkill = 151;
	int32_t m_ImitationSkunkStinkSkill = 50;
	float m_RecheckTime = 0.5f;
	float m_PathingTime = 100.0f;
};

#endif  //!BALLOON_H
