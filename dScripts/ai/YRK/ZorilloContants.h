#ifndef __ZorilloContants__H__
#define __ZorilloContants__H__

#include <cstdint>
#include <string>
#include <vector>

#include "NiPoint3.h"
#include "dCommonVars.h"

namespace ZorilloConstants {
	constexpr uint32_t skunkStinkSkill = 33;
	constexpr uint32_t destinkSkill = 116;
	constexpr uint32_t removeStinkSkill = 124;

	constexpr LOT singleLampLot = 3180;
	constexpr LOT spoutLot = 3283;
	constexpr LOT lampDetectorLot = 3434;
	constexpr LOT earthquakeCenterLot = 3378;
	constexpr LOT fountainAlertLot = 3674;
	constexpr LOT invasionStinkCloudLot = 3851;
	constexpr LOT inventorBuildingLot = 3172;
	constexpr LOT switchThrowerLot = 3923;
	constexpr LOT hazmatVanLot = 3472;
	constexpr LOT hazmatRebuildVanLot = 3717;
	constexpr LOT bubbleBlowerLot = 3928;
	constexpr LOT airStinkLot = 3645;
	constexpr LOT spawnedHazmatNpc = 3553;
	constexpr LOT poleSlideNpc = 3954;
	constexpr LOT balloonLot = 3433;
	constexpr LOT flowerLot = 3646;

	constexpr LOT skunk3279 = 3279;
	constexpr LOT skunk3930 = 3930;
	constexpr LOT skunk3931 = 3931;
	const std::vector<LOT> invasionSkunkLot = { skunk3279, skunk3930, skunk3931 };

	const std::vector<LOT> invasionPanicActors = { 3268, 3269, 3270, 3271, 3272 };

	constexpr uint32_t cleaningPointsTotal = 50;
	constexpr uint32_t cleaningPointsMedium = 20;
	constexpr uint32_t cleaningPointsLow = 40;
	constexpr uint32_t pointValueSkunk = 1;
	constexpr uint32_t pointValueStinkCloud = 1;
	constexpr uint32_t pointValueBroombot = 3;
	constexpr uint32_t pointValueHazmat = 2;
	constexpr uint32_t rewardMultiplier = 10;

	constexpr uint32_t numSkunks = 10;
	const std::string skunkPathPrefix = "skunkWP_";
	const std::string skunkRoamPathSuffix = "a";
	constexpr uint32_t numStinkClouds = 10;
	const std::string stinkCloudPath = "StinkCloudSpawnLocations";
	constexpr uint32_t skunkRespawnTimerMin = 5;
	constexpr uint32_t skunkRespawnTimerMax = 10;

	const std::string hazmatRebuildVanSpawnPath = "HazmatRebuildSpawnPath";
	const std::string hazmatNpcPathPrefix = "hazmatWP_";
	constexpr uint32_t numHazmatNpcs = 4;
	constexpr float timeBetweenHazmatSpawns = 4.0f;
	constexpr float hazmatRebuildResetTime = 20.0f;

	constexpr float peaceTimeDuration = 20;
	constexpr float invasionTransitionDuration = 12.0;
	constexpr float doneTransitionDuration = 5.0;
	constexpr float maxInvasionDuration = 5 * 60.0;
	constexpr float earthquakeDuration = 2.5;
	constexpr float fountainAlertTiming = 4.0;
	constexpr float skunkSpawnTiming = 10.0;
	constexpr float hazmatVanTiming = 11.0;
	constexpr float poleSlideTiming = 11.0;
	constexpr float hazmatNpcSpawnTimer = 1.0;

	const std::string stinkySkybox = "mesh/env/env_sky_won_yore_skunk-stink.nif";
	const std::string normalSkybox = "mesh/env/challenge_sky_light_2awesome.nif";
	const std::string skylayer = "(invalid)";
	const std::string ringlayer0 = "(invalid)";
	const std::string ringlayer1 = "(invalid)";
	const std::string ringlayer2 = "(invalid)";
	const std::string ringlayer3 = "(invalid)";

	constexpr uint32_t hfNodeBouncer = 7;
	constexpr char consthfSubItemSepString = 0x1f;

	constexpr float spoutRadius = 2.0;
	const std::string spoutGroupName = "spoutGroup";
	const std::string fountainGroupName = "fountainGroup";
	constexpr float spoutBouncerSpeed = 100.0;
	const NiPoint3 spoutBouncerDest(-12.88, 318.21, -124.52);

	constexpr float bubbleStatueRadius = 10.0;

	constexpr uint32_t lastBalloonWaypoint = 11;

	constexpr float radius = 3.0;

	// Custom constants
	const std::string zoneChangeNotification = "zone_state_change";
	const std::u16string zoneChangeNotificationu16 = u"zone_state_change";
};

enum class SkunkEventState: int32_t {
	ZoneStateNoInfo = -1,
	ZoneStateNoInvasion,
	ZoneStateTransition,
	ZoneStateHighAlert,
	ZoneStateMediumAlert,
	ZoneStateLowAlert,
	ZoneStateDoneTransition
};

#endif  //!__ZorilloContants__H__
