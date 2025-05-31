// when a LOT 20171 approaches (let's say gets within a radius of 4), kill it, and play "attack" animation on self

#ifndef NP_PICNIC_BOT_H
#define NP_PICNIC_BOT_H
#include "CppScripts.h"
#include "Entity.h"

class NpPicnicBot : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
	void OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) override;
private:
	int m_EnemyLot = 20171;
};

#endif // NP_PICNIC_BOT_H
