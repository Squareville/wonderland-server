set(DSCRIPTS_SOURCES_AI_YRK
	"BabySkunks.cpp"
	"BalloonTrigger.cpp"
	"HazmatMissionGiver.cpp"
	"YrkActor.cpp"
	"YrkNpcOntimer.cpp")

add_library(dScriptsAiYrk OBJECT ${DSCRIPTS_SOURCES_AI_YRK})
target_include_directories(dScriptsAiYrk PUBLIC ".")
target_precompile_headers(dScriptsAiYrk REUSE_FROM dScriptsBase)
