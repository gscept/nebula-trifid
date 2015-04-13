#-------------------------------------------------------------------------------
# archiver
#-------------------------------------------------------------------------------

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("archiver3" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${PCH_files})

ADD_EXECUTABLE(archiver3 ${ALLSRCS})
SET_TARGET_PROPERTIES(archiver3 PROPERTIES FOLDER "N3SDK/Toolkit")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(archiver3)
N_SET_EXE_SUBSYSTEM(archiver3 Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(archiver3 stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(archiver3 toolkitutil)
TARGET_LINK_LIBRARIES(archiver3 toolkitutil)
