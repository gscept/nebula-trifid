#-------------------------------------------------------------------------------
# modelbatcher
#-------------------------------------------------------------------------------

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("modelbatcher3" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${PCH_files})

ADD_EXECUTABLE(modelbatcher3 ${ALLSRCS})
SET_TARGET_PROPERTIES(modelbatcher3 PROPERTIES FOLDER "N3SDK/Toolkit")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(modelbatcher3)
N_SET_EXE_SUBSYSTEM(modelbatcher3 Console)

IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(modelbatcher3 stdneb.h stdneb.cc ../)
ENDIF()

ADD_DEPENDENCIES(modelbatcher3 toolkitutil)
TARGET_LINK_LIBRARIES(modelbatcher3 toolkitutil)
