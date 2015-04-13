#-------------------------------------------------------------------------------
# Texturebatcher
#-------------------------------------------------------------------------------

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("texturebatcher3" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${PCH_files})

ADD_EXECUTABLE(texturebatcher3 ${ALLSRCS})
SET_TARGET_PROPERTIES(texturebatcher3 PROPERTIES FOLDER "N3SDK/Toolkit")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(texturebatcher3)
N_SET_EXE_SUBSYSTEM(texturebatcher3 Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(texturebatcher3 stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(texturebatcher3
	toolkitutil	
)
TARGET_LINK_LIBRARIES(texturebatcher3 
	toolkitutil
)
