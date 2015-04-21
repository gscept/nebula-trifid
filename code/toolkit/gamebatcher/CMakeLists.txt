#-------------------------------------------------------------------------------
# gamebatcher
#-------------------------------------------------------------------------------

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("gamebatcher" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${SRCSSPLITTER} ${PCH_files})

ADD_EXECUTABLE(gamebatcher ${ALLSRCS} ${SOURCES_NIDLS})
SET_TARGET_PROPERTIES(gamebatcher PROPERTIES FOLDER "N3SDK/Toolkit")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(gamebatcher)
N_SET_EXE_SUBSYSTEM(gamebatcher Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(gamebatcher stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(gamebatcher toolkitutil )
TARGET_LINK_LIBRARIES(gamebatcher toolkitutil )
