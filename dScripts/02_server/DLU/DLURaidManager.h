#pragma once

#include "CppScripts.h"

#include "Raid.h"

class DLURaidManager : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
	void OnUse(Entity* self, Entity* user) override;
	void OnUpdate(Entity* self) override;

	void EndPreperationStage();
	
	void SetRaid(Raid raid);
private:
	std::vector<Entity*> m_PlayerLeaders;

	Raid m_Raid;
};
