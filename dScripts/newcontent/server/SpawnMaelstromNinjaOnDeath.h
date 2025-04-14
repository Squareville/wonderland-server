#ifndef SPAWNMAELSTROMNINJAONDEATH_H
#define SPAWNMAELSTROMNINJAONDEATH_H

#include "CppScripts.h"

class SpawnMaelstromNinjaOnDeath : public CppScripts::Script
{
public:
	void OnDie(Entity* self, Entity* killer) override;
};

#endif //!SPAWNMAELSTROMNINJAONDEATH_H
