#include "NpAnt.h"

#include "Entity.h"
#include "InventoryComponent.h"

void NpAnt::OnWaypointReached(Entity* self, uint32_t waypointIndex) {
	// Get inventory component
	auto* inventoryComponent = self->GetComponent<InventoryComponent>();
	if (!inventoryComponent) return;
	
	if (waypointIndex == 0) {
		auto index = GeneralUtils::GenerateRandomNumber<int>(0, 2);

		auto item = inventoryComponent->FindItemByLot(m_ItemsToEquip[index]);
		if (!item) return;

		// Equip the item
		m_EquippedItem = item;
		inventoryComponent->EquipItem(m_EquippedItem);
	} else if (waypointIndex == 2) {
		if (!m_EquippedItem) return;
		// Unequip any equipped item
		inventoryComponent->UnEquipItem(m_EquippedItem);
	}
}