set(DSCRIPTS_SOURCES_AI_YRK
	"BalloonTrigger.cpp"
	"BubbleStatue.cpp"
	"EnemySkunk.cpp"
	"HazmatMissionGiver.cpp"
	"HazmatTruck.cpp"
	"SkunkEvent.cpp"
	"Spout.cpp"
	"YrkActor.cpp"
	"YrkNpcOnTimer.cpp"
	"YrkNpcStink.cpp"
	"MechEuBroombot.cpp")

add_library(dScriptsAiYrk OBJECT ${DSCRIPTS_SOURCES_AI_YRK})
target_include_directories(dScriptsAiYrk PUBLIC ".")
target_precompile_headers(dScriptsAiYrk REUSE_FROM dScriptsBase)
