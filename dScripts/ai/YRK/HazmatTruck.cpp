#include "HazmatTruck.h"

#include "Entity.h"
#include "RebuildComponent.h"
#include "ZorilloContants.h"
#include "dCommonVars.h"

void HazmatTruck::OnRebuildNotifyState(Entity* self, eRebuildState state) {
	self->CancelAllTimers();
	if (state == eRebuildState::REBUILD_OPEN) {
		self->AddTimer("BreakTimer", ZorilloConstants::hazmatRebuildResetTime);
	}
}

void HazmatTruck::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName == "BreakTimer") {
		auto* rebuildComponent = self->GetComponent<RebuildComponent>();
		if (rebuildComponent) rebuildComponent->ResetRebuild(false);
	}
}