#include "SkunkEvent.h"

#include "Game.h"
#include "GameMessages.h"
#include "SkunkEventState.h"
#include "Zone.h"
#include "dZoneManager.h"
#include "dLogger.h"

void SkunkEvent::OnStartup(Entity* self) {
	InitZoneVars(self);

	// -- spawn in the garage van
	// spawnGarageVan(self)

	// -- start a timer to spawn in trip build animals
	// GAMEOBJ:GetTimer():AddTimerWithCancel( 10, "sporeTimer", self )

	// -- start the first timer for the event    
	// GAMEOBJ:GetTimer():AddTimerWithCancel( CONSTANTS["PEACE_TIME_DURATION"], "startEventTimer", self )
}

void SkunkEvent::InitZoneVars(Entity* self) {
	auto* stinkPath = dZoneManager::Instance()->GetZone()->GetPath("StinkCloudSpawnLocations");
	self->SetVar<uint32_t>(u"NumStinkCloudSpawnPoints", stinkPath->pathWaypoints.size());
	for (auto path : stinkPath->pathWaypoints) {
		Game::logger->Log("SkunkEvent", "waypoint is %f %f %f", path.position.x, path.position.y, path.position.z);
	}

	SetZoneState(self, SkunkEventState::ZONE_STATE_NO_INVASION);
	// ResetTotalCleanPoints(self)
}

void SkunkEvent::SetZoneState(Entity* self, SkunkEventState state) {
	Game::logger->Log("SkunkEvent", "new zone state is %i", state);
	SkunkEventState previousState = GetZoneState(self);

	self->SetVar(m_ZoneState, state);

	if (previousState != state) {
		switch (state) {
			case SkunkEventState::ZONE_STATE_NO_INVASION:
			//  DoNoInvasionStateActions(self)
			break;
			case SkunkEventState::ZONE_STATE_TRANSITION:
			//  DoTransitionStateActions(self)
			break;
			case SkunkEventState::ZONE_STATE_HIGH_ALERT:
			//  DoHighAlertStateActions(self)
			break;
			case SkunkEventState::ZONE_STATE_DONE_TRANSITION:
			//  DoDoneTransitionActions(self)
			break;
			
			case SkunkEventState::ZONE_STATE_MEDIUM_ALERT:
			case SkunkEventState::ZONE_STATE_LOW_ALERT:
			case SkunkEventState::ZONE_STATE_NO_INFO:
			break;
			// Do nothing!
		}
	}
}

void SkunkEvent::OnPlayerLoaded(Entity* self, Entity* player) {
	Game::logger->Log("SkunkEvent", "notifying client!");
	GameMessages::SendNotifyClientZoneObject(self->GetObjectID(), u"zone_state_change", static_cast<int32_t>(SkunkEventState::ZONE_STATE_HIGH_ALERT), 0, LWOOBJID_EMPTY, "", player->GetSystemAddress());
	GameMessages::SendNotifyClientZoneObject(dZoneManager::Instance()->GetZoneControlObject()->GetObjectID(), u"zone_state_change", static_cast<int32_t>(SkunkEventState::ZONE_STATE_HIGH_ALERT), 0, LWOOBJID_EMPTY, "", player->GetSystemAddress());
	for (auto e : EntityManager::Instance()->GetEntitiesByLOT(3928)) {
		e->NotifyObject(self, "zone_state_change", static_cast<int32_t>(SkunkEventState::ZONE_STATE_HIGH_ALERT));
	}
}

SkunkEventState SkunkEvent::GetZoneState(Entity* self) {
	return self->GetVar<SkunkEventState>(m_ZoneState);
}
