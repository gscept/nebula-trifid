#-------------------------------------------------------------------------------
# gamebatcher
#-------------------------------------------------------------------------------

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("gamebatcher3" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${SRCSSPLITTER} ${PCH_files})

ADD_EXECUTABLE(gamebatcher3 ${ALLSRCS} ${SOURCES_NIDLS})
SET_TARGET_PROPERTIES(gamebatcher3 PROPERTIES FOLDER "N3SDK/Toolkit")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(gamebatcher3)
N_SET_EXE_SUBSYSTEM(gamebatcher3 Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(gamebatcher3 stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(gamebatcher3 toolkitutil )
TARGET_LINK_LIBRARIES(gamebatcher3 toolkitutil )
