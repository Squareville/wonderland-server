#include "CppScripts.h"

class BabySkunk : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
	void OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) override;
	void OnTimerDone(Entity* self, std::string timerName) override;
	void OnArrived(Entity* self);
	void OnStoppedEvading(Entity* self);
	void OnLeftTetherRadius(Entity* self);
};
