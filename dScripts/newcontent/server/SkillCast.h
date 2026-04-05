#include "CppScripts.h"

class SkillCast: public CppScripts::Script {
public:
	/**
	 * A script that casts a skill on the entity.
	 * 
	 * @param skillToCast The skill to castr
	 */
	SkillCast(int32_t skillToCast = 0)
	: m_SkillToCast(skillToCast) {}
	void OnStartup(Entity* self) override;
private:
	int32_t m_SkillToCast;
};
