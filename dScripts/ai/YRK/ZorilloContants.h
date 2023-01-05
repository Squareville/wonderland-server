#ifndef __ZorilloContants__H__
#define __ZorilloContants__H__

#include <cstdint>
#include <string>
#include <vector>

#include "NiPoint3.h"
#include "dCommonVars.h"

namespace ZorilloConstants {
	const uint32_t skunkStinkSkill = 33;
	const uint32_t destinkSkill = 116;
	const uint32_t removeStinkSkill = 124;

	const LOT singleLampLot = 3180;
	const LOT spoutLot = 3283;
	const LOT lampDetectorLot = 3434;
	const LOT earthquakeCenterLot = 3378;
	const LOT fountainAlertLot = 3674;
	const LOT invasionStinkCloudLot = 3851;
	const LOT inventorBuildingLot = 3172;
	const LOT switchThrowerLot = 3923;
	const LOT hazmatVanLot = 3472;
	const LOT hazmatRebuildVanLot = 3717;
	const LOT bubbleBlowerLot = 3928;
	const LOT airStinkLot = 3645;
	const LOT spawnedHazmatNpc = 3553;
	const LOT poleSlideNpc = 3954;
	const LOT balloonLot = 3433;
	const LOT flowerLot = 3646;

	const LOT skunk3279 = 3279;
	const LOT skunk3930 = 3930;
	const LOT skunk3931 = 3931;
	const std::vector<LOT> invasionSkunkLot = { skunk3279, skunk3930, skunk3931 };

	const std::vector<LOT> invasionPanicActors = { 3268, 3269, 3270, 3271, 3272 };

	const uint32_t cleaningPointsTotal = 50;
	const uint32_t cleaningPointsMedium = 20;
	const uint32_t cleaningPointsLow = 40;
	const uint32_t pointValueSkunk = 1;
	const uint32_t pointValueStinkCloud = 1;
	const uint32_t pointValueBroombot = 3;
	const uint32_t pointValueHazmat = 2;
	const uint32_t rewardMultiplier = 10;

	const uint32_t numSkunks = 10;
	const std::string skunkPathPrefix = "skunkWP_";
	const std::string skunkRoamPathSuffix = "a";
	const uint32_t numStinkClouds = 10;
	const std::string stinkCloudPath = "StinkCloudSpawnLocations";
	const uint32_t skunkRespawnTimerMin = 5;
	const uint32_t skunkRespawnTimerMax = 10;

	const std::string hazmatRebuildVanSpawnPath = "HazmatRebuildSpawnPath";
	const std::string hazmatNpcPathPrefix = "hazmatWP_";
	const uint32_t numHazmatNpcs = 4;
	const float timeBetweenHazmatSpawns = 4.0f;
	const float hazmatRebuildResetTime = 20.0f;

	const float peaceTimeDuration = 20;
	const float invasionTransitionDuration = 12.0;
	const float doneTransitionDuration = 5.0;
	const float maxInvasionDuration = 5 * 60.0;
	const float earthquakeDuration = 2.5;
	const float fountainAlertTiming = 4.0;
	const float skunkSpawnTiming = 10.0;
	const float hazmatVanTiming = 11.0;
	const float poleSlideTiming = 11.0;
	const float hazmatNpcSpawnTimer = 1.0;

	const std::string stinkySkybox = "mesh/env/env_sky_won_yore_skunk-stink.nif";
	const std::string normalSkybox = "mesh/env/challenge_sky_light_2awesome.nif";
	const std::string skylayer = "(invalid)";
	const std::string ringlayer0 = "(invalid)";
	const std::string ringlayer1 = "(invalid)";
	const std::string ringlayer2 = "(invalid)";
	const std::string ringlayer3 = "(invalid)";

	const uint32_t hfNodeBouncer = 7;
	const char consthfSubItemSepString = 0x1f;

	const float spoutRadius = 2.0;
	const std::string spoutGroupName = "spoutGroup";
	const std::string fountainGroupName = "fountainGroup";
	const float spoutBouncerSpeed = 100.0;
	const NiPoint3 spoutBouncerDest(-12.88, 318.21, -124.52);

	const float bubbleStatueRadius = 10.0;

	const uint32_t lastBalloonWaypoint = 11;

	const float radius = 3.0;

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
