#ifndef SKUNKEVENT_H
#define SKUNKEVENT_H

#include "CppScripts.h"

#include "magic_enum.hpp"

enum class SkunkEventZoneState : int32_t {
	NO_INVASION,
	TRANSITION,
	HIGH_ALERT,
	MEDIUM_ALERT,
	LOW_ALERT,
	DONE_TRANSITION,
	NO_INFO = -1,
};

template <>
struct magic_enum::customize::enum_range<SkunkEventZoneState> {
	static constexpr int min = -1;
	static constexpr int max = 5;
};

class SkunkEvent : public CppScripts::Script {
public:
	// Constants, and oh boy are there a lot of them
	static constexpr auto NO_OBJECT = "0";
	static constexpr auto EMPTY_ID_NAME = "|NO_OBJECT";
	static constexpr auto LOT_NULL = -1;
	static constexpr auto SKUNK_STINK_SKILL = 33;
	static constexpr auto DESTINK_SKILL = 116;
	static constexpr auto REMOVE_STINK_SKILL = 124;
	static constexpr auto SINGLE_LAMP_LOT = 3180;
	static constexpr auto SPOUT_LOT = 3283;
	static constexpr auto LAMP_DETECTOR_LOT = 3434;
	static constexpr auto EARTHQUAKE_CENTER_LOT = 3378;
	static constexpr auto FOUNTAIN_ALERT_LOT = 3674;
	static constexpr auto INVASION_STINK_CLOUD_LOT = 3851;
	static constexpr auto INVENTOR_BUILDING_LOT = 3172;
	static constexpr auto SWITCH_THROWER_LOT = 3923;
	static constexpr auto HAZMAT_VAN_LOT = 3472;
	static constexpr auto HAZMAT_REBUILD_VAN_LOT = 3717;
	static constexpr auto BUBBLE_BLOWER_LOT = 3928;
	static constexpr auto AIR_STINK_LOT = 3645;
	static constexpr auto SPAWNED_HAZMAT_NPC = 3553;
	static constexpr auto POLE_SLIDE_NPC = 3954;
	static constexpr auto BALLOON_LOT = 3433;
	static constexpr auto INVASION_SKUNK_LOT = { 3279, 3930, 3931 };
	static constexpr auto INVASION_PANIC_ACTORS = { 3268, 3269, 3270, 3271, 3272 };
	static constexpr auto CLEANING_POINTS_TOTAL = 50;
	static constexpr auto CLEANING_POINTS_MEDIUM = 20;
	static constexpr auto CLEANING_POINTS_LOW = 40;
	static constexpr auto POINT_VALUE_SKUNK = 1;
	static constexpr auto POINT_VALUE_STINK_CLOUD = 1;
	static constexpr auto POINT_VALUE_BROOMBOT = 3;
	static constexpr auto POINT_VALUE_HAZMAT = 2;
	static constexpr auto REWARD_MULTIPLIER = 10;
	static constexpr auto NUM_SKUNKS = 10;
	static constexpr auto SKUNK_PATH_PREFIX = "skunkWP_";
	static constexpr auto SKUNK_ROAM_PATH_SUFFIX = "a";
	static constexpr auto NUM_STINK_CLOUDS = 10;
	static constexpr auto STINK_CLOUD_PATH = "StinkCloudSpawnLocations";
	static constexpr auto SKUNK_RESPAWN_TIMER_MIN = 5;
	static constexpr auto SKUNK_RESPAWN_TIMER_MAX = 10;
	static constexpr auto HAZMAT_REBUILD_VAN_SPAWN_PATH = "HazmatRebuildSpawnPath";
	static constexpr auto HAZMAT_NPC_PATH_PREFIX = "hazmatWP_";
	static constexpr auto NUM_HAZMAT_NPCS = 4;
	static constexpr auto TIME_BETWEEN_HAZMAT_SPAWNS = 4.0f;
	static constexpr auto HAZMAT_REBUILD_RESET_TIME = 20.0f;
	static constexpr auto PEACE_TIME_DURATION = 5 * 60;
	static constexpr auto INVASION_TRANSITION_DURATION = 12.0f;
	static constexpr auto DONE_TRANSITION_DURATION = 5.0f;
	static constexpr auto MAX_INVASION_DURATION = 5 * 60.0f;
	static constexpr auto EARTHQUAKE_DURATION = 2.5f;
	static constexpr auto FOUNTAIN_ALERT_TIMING = 4.0f;
	static constexpr auto SKUNK_SPAWN_TIMING = 10.0f;
	static constexpr auto HAZMAT_VAN_TIMING = 11.0f;
	static constexpr auto POLE_SLIDE_TIMING = 11.0f;
	static constexpr auto HAZMAT_NPC_SPAWN_TIMER = 1.0f;
	static constexpr auto STINKY_SKYBOX = "mesh/env/env_sky_won_yore_skunk-stink.nif";
	static constexpr auto NORMAL_SKYBOX = "mesh/env/challenge_sky_light_2awesome.nif";
	static constexpr auto SKYLAYER = "(invalid)";
	static constexpr auto RINGLAYER0 = "(invalid)";
	static constexpr auto RINGLAYER1 = "(invalid)";
	static constexpr auto RINGLAYER2 = "(invalid)";
	static constexpr auto RINGLAYER3 = "(invalid)";
	static constexpr auto HF_NODE_BOUNCER = 7;
	static constexpr auto HF_SUB_ITEM_SEP_STRING = "\x1F";
	static constexpr auto SPOUT_RADIUS = 2.0f;
	static constexpr auto SPOUT_GROUP_NAME = "spoutGroup";
	static constexpr auto FOUNTAIN_GROUP_NAME = "fountainGroup";
	static constexpr auto SPOUT_BOUNCER_SPEED = 100.0f;
	static constexpr NiPoint3 SPOUT_BOUNCER_DEST = { -12.88f, 318.21f, -124.52f };
	static constexpr auto BUBBLE_STATUE_RADIUS = 10.0f;
	static constexpr auto LAST_BALLOON_WAYPOINT = 11;
	static constexpr auto radius = 3.0f;
	void OnStartup(Entity* self) override;
	void OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1 = 0, int32_t param2 = 0) override;
	void OnPlayerLoaded(Entity* self, Entity* player) override;
	void OnFireEventServerSide(Entity* self, Entity* sender, std::string args, int32_t param1, int32_t param2, int32_t param3) override;
	void OnTimerDone(Entity* self, std::string name) override;
	void OnObjectLoaded(Entity* self, LWOOBJID objId, LOT lot);
	void OnChildLoaded(Entity* self, const LWOOBJID objectId, const LOT lot) override;
private:
	bool InvasionActive(const Entity* const self) const;
	SkunkEventZoneState GetZoneState(const Entity* const self) const;
	void SetZoneState(Entity* const self, const SkunkEventZoneState state) const;

	// Return true if we are transitioning to the done state.
	bool IncrementTotalCleanPoints(Entity* const self, const int32_t points) const;
	void SpawnSingleStinkCloud(Entity* const self, const int32_t number) const;
	bool IsValidWaypoint(const Entity* const self, const int32_t waypoint) const;
	void AddPlayerPoints(const Entity* const self, const LWOOBJID player, const int32_t points) const;
	void ResetTotalCleanPoints(Entity* const self) const;
	void InitZoneVars(Entity* const self) const;
	void SpawnGarageVan(Entity* const self) const;
	bool IsValidNpc(const Entity* const self, const LOT lot) const;
	bool IsValidSkunk(const Entity* const self, const LOT lot) const;
	void NotifyNpcs(Entity* const self, const std::string& name) const;
	void DoNoInvasionStateActions(Entity* const self) const;
	void DoTransitionStateActions(Entity* const self) const;
	void DoHighAlertStateActions(Entity* const self) const;
	void DoDoneTransitionActions(Entity* const self) const;
	void SendStateToEntities(Entity* const self, const std::vector<LWOOBJID>& entities) const;
	void KillSkunks(Entity* const self) const;
	void KillStinkClouds(Entity* const self) const;
	void KillHazmatNpcs(Entity* const self) const;
	float AnimateVan(Entity* const self, const std::string& animName) const;
	void KillEntities(Entity* const self, const std::vector<LWOOBJID>& entities) const;
	void RewardPlayers(Entity* const self) const;
	void SpawnRebuildVan(Entity* const self) const;
	void SpawnStinkClouds(Entity* const self) const;
	void SpawnHazmatNpcs(Entity* const self) const;
	void SpawnSkunks(Entity* const self) const;
	void SpawnSingleHazmatNpc(Entity* const self, const std::string& pathStr) const;
	void SpawnSingleSkunk(Entity* const self, const int32_t num, const bool respawn) const;
};

#endif  //!SKUNKEVENT_H
