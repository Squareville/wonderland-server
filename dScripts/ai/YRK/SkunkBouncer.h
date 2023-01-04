#ifndef __SKUNKBOUNCER__H__
#define __SKUNKBOUNCER__H__

#include "CppScripts.h"

class SkunkBouncer : public CppScripts::Script {
public:
	void OnCollisionPhantom(Entity* self, Entity* target) override;
private:
	void BounceNow(Entity* self, Entity* target);
	bool IsTargetASkunk(Entity* self, Entity* target);
	bool IsTargetABabySkunk(Entity* target);
	bool IsTargetAnAdultSkunk(Entity* target);
	void AddStinkEffectToTarget(Entity* target);
	void AddStinkEffectToBouncer(Entity* target);
	void CheckForStink(Entity* self, Entity* target);
private:
	const uint32_t STINKY_SKUNK_SKILL = 109;
	const LOT m_BabySkunk = 3476;
	const uint32_t m_BouncerStinkFxEffectId = 337;
	const uint32_t m_SkunkStinkFxEffectId = 338;
	const std::vector<LOT> m_AdultSkunk = { 3279, 3930, 3931 };
	const std::u16string m_SkunkEffectName = "skunkBouncer";
	const std::u16string m_BabySkunkEffectName = "skunkBouncerBaby";
	const std::u16string m_AdultSkunkEffectName = "skunkBouncerAdult";
};

#endif  //!__SKUNKBOUNCER__H__
