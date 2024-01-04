set(DSCRIPTS_SOURCES_NEWCONTENT)

add_subdirectory(server)

foreach(file ${DSCRIPTS_SOURCES_NEWCONTENT_SERVER})
	set(DSCRIPTS_SOURCES_NEWCONTENT ${DSCRIPTS_SOURCES_NEWCONTENT} "server/${file}")
endforeach()

add_library(dScriptsNewContent STATIC ${DSCRIPTS_SOURCES_NEWCONTENT})
target_include_directories(dScriptsNewContent PUBLIC "." "server")
target_precompile_headers(dScriptsNewContent REUSE_FROM dScriptsBase)
