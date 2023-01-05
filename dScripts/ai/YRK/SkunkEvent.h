#ifndef __SKUNKEVENT__H__
#define __SKUNKEVENT__H__

#include <cstdint>
#include <string>

#include "CppScripts.h"

class Entity;

enum class SkunkEventState : int32_t;

class SkunkEvent : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
	void OnChildLoaded(Entity* self, Entity* child);
	void OnObjectLoaded(Entity* self, Entity* loadedEntity);
	void OnPlayerLoaded(Entity* self, Entity* player) override;
	void OnTimerDone(Entity* self, std::string timer) override;
	void OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1 = 0, int32_t param2 = 0) override;
	void OnFireEventServerSide(Entity* self, Entity* sender, std::string args, int32_t param1, int32_t param2, int32_t param3) override;
private:
	void OnRequestFollow(Entity* self, Entity* requestor);
	bool IsInvasionActive(Entity* self);
	SkunkEventState GetZoneState(Entity* self);
	void SetZoneState(Entity* self, SkunkEventState state);
	void DoNoInvasionStateActions(Entity* self);
	void DoTransitionStateActions(Entity* self);
	void DoHighAlertStateActions(Entity* self);
	void DoMediumAlertStateActions(Entity* self);
	void DoLowAlertStateActions(Entity* self);
	void DoDoneTransitionActions(Entity* self);
	void SendStateToSpouts(Entity* self);
	void SendStateToBubbleBlowers(Entity* self);
	uint32_t GetTotalCleanPoints(Entity* self);
	bool IncrementTotalCleanPoints(Entity* self, uint32_t incrementedPoints);
	void ResetTotalCleanPoints(Entity* self);
	void InitZoneVars(Entity* self);
	void SpawnGarageVan(Entity* self);
	void SpawnRebuildVan(Entity* self);
	void SpawnSkunks(Entity* self);
	void SpawnSingleSkunk(Entity* self, uint32_t num, bool respawn);
	uint32_t GetRandomWaypoint(std::string path);
	void KillSkunks(Entity* self);
	bool IsWaypointValid(Entity* self, uint32_t waypoint);
	void SpawnStinkClouds(Entity* self);
	void SpawnSingleStinkCloud(Entity* self, uint32_t num);
	void KillStinkClouds(Entity* self);
	void AddPlayerPoints(Entity* self, Entity* player, uint32_t points);
	void RewardPlayers(Entity* self);
	void EndSkunkEvent(Entity* self);
	void PanicNPCs(Entity* self);
	void IdleNPCs(Entity* self);
	bool IsValidNPC(LOT templateId);
	bool IsValidSkunk(LOT templateId);
	float AnimateVan(Entity* self, std::u16string name);
	void SpawnHazmatNPCs(Entity* self);
	void SpawnSingleHazmatNPC(Entity* self, uint32_t num);
	void KillHazmatNPCs(Entity* self);
	void CancelAllTimers(Entity* self);
	void LoadSporeAnimals(Entity* self);

	std::map<uint32_t, Entity*> invasionSkunks{};
	std::map<uint32_t, Entity*> hazmatNpcs{};
	std::map<uint32_t, Entity*> invationStinkClouds{};
	std::map<uint32_t, Entity*> spawnedNpcs{};
	std::map<uint32_t, Entity*> spawnedSpouts{};
	std::map<uint32_t, Entity*> spawnedBubbleBlowers{};
	std::map<uint32_t, Entity*> spawnedFlowers{};
	std::vector<Entity*> invasionPlayers{};
	std::map<uint32_t, uint32_t> spawnedStinkCloudWaypoints{};

	const std::u16string m_ZoneState = u"ZoneState";
};

#endif  //!__SKUNKEVENT__H__
