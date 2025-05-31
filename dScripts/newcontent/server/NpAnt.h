#ifndef NP_ANT_H
#define NP_ANT_H

#include "CppScripts.h"

class NpAnt : public CppScripts::Script {
public:
	void OnWaypointReached(Entity* self, uint32_t waypointIndex) override;
private:
	std::vector<LOT> m_ItemsToEquip = { 3390, 3391, 3392 }; // List of item IDs to equip
	Item* m_EquippedItem = nullptr; // Currently equipped item, if any
};

#endif // NP_ANT_H