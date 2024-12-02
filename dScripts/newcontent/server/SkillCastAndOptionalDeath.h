#include "CppScripts.h"

class SkillCastAndOptionalDeath : public CppScripts::Script {
public:
	/**
	 * A script that casts a skill on the entity.
	 * Optionally kills the entity after X seconds.
	 * 
	 * @param skillToCast The skill to cast
	 * @param dieAfter2XSeconds Whether to kill the entity after the specified seconds
	 * @param timeToDie how long to kill the entity after
	 */
	SkillCastAndOptionalDeath(int32_t skillToCast = 0, bool dieAfterXSeconds = false, float timeToDie = 20.0f)
	: m_SkillToCast(skillToCast), m_DieAfterXSeconds(dieAfterXSeconds), m_TimeToDie(timeToDie) {}
	void OnStartup(Entity* self) override;
private:
	int32_t m_SkillToCast;
	bool m_DieAfterXSeconds;
	float m_TimeToDie;
};
