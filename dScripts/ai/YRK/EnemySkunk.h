#ifndef ENEMYSKUNK_H
#define ENEMYSKUNK_H

#include "CppScripts.h"

class EnemySkunk : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
	void OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) override;
	void OnSkillEventFired(Entity* self, Entity* caster, const std::string& message) override;
};

#endif  //!ENEMYSKUNK_H
