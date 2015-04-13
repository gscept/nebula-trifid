#-------------------------------------------------------------------------------
# audiobatcher
#-------------------------------------------------------------------------------

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("audiobatcher3" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${PCH_files})

ADD_EXECUTABLE(audiobatcher3 ${ALLSRCS})
SET_TARGET_PROPERTIES(audiobatcher3 PROPERTIES FOLDER "N3SDK/Toolkit")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(audiobatcher3)
N_SET_EXE_SUBSYSTEM(audiobatcher3 Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(audiobatcher3 stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(audiobatcher3 toolkitutil)
TARGET_LINK_LIBRARIES(audiobatcher3 toolkitutil	)
