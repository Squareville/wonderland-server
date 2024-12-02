#include "CppScripts.h"

class HalloweenManager : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
	void SpawnTheBossSmashable(Entity* self);
	void HandleTheBossSmashableDeath(Entity* self);

private:
	LOT m_Horseman = 21512;
	LOT m_Vampire = 30057;
	LOT m_Mummy = 30058;

	// Location A
	NiPoint3 m_SpawnLocationA = {-12.82, 291.7, -124.92};
	NiQuaternion m_SpawnRotationA = {0.9921146631240845, 0.0, -0.12533347308635712, 0.0};

	// Location B
	NiPoint3 m_SpawnLocationB = {-12.82, 291.8, -124.92};
	NiQuaternion m_SpawnRotationB = {0.5180241465568542, 0.0, 0.8553661108016968, 0.0};
};
