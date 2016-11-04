#-------------------------------------------------------------------------------
# clouds
#-------------------------------------------------------------------------------
	
set(SOURCES_files_clouds
	cloudsaddon.h
	cloudsaddon.cc
	cloudsrtplugin.h
	cloudsrtplugin.cc
)
source_group("clouds" FILES ${SOURCES_files_clouds})

set(SOURCES_clouds
    ${SOURCES_files_clouds}
)

set(SOURCES_files
../stdneb.h
../stdneb.cc
)
	
SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
set(SOURCES_SRCS
	${SOURCES_clouds}
	${SOURCES_files}
)

add_library( clouds STATIC ${SOURCES_SRCS} )
TARGET_LINK_LIBRARIES(clouds PUBLIC render)
SET_TARGET_PROPERTIES(clouds PROPERTIES FOLDER "N3SDK/addons")
N_SET_COMPILER_DEFAULTS(clouds)
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(clouds stdneb.h stdneb.cc ../)
ENDIF()
