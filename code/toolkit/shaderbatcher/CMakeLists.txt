#-------------------------------------------------------------------------------
# shaderbatcher
#-------------------------------------------------------------------------------

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("shaderbatcher" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${PCH_files})

ADD_EXECUTABLE(shaderbatcher ${ALLSRCS})
SET_TARGET_PROPERTIES(shaderbatcher PROPERTIES FOLDER "N3SDK/Toolkit")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(shaderbatcher)
N_SET_EXE_SUBSYSTEM(shaderbatcher Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(shaderbatcher stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(shaderbatcher
	toolkitutil
	)
TARGET_LINK_LIBRARIES(shaderbatcher 		
	toolkitutil
)
