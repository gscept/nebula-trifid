#-------------------------------------------------------------------------------
# assetbatcher
#-------------------------------------------------------------------------------

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("assetbatcher" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${SRCSSPLITTER} ${PCH_files})

ADD_EXECUTABLE(assetbatcher ${ALLSRCS})
SET_TARGET_PROPERTIES(assetbatcher PROPERTIES FOLDER "N3SDK/Toolkit")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(assetbatcher)
N_SET_EXE_SUBSYSTEM(assetbatcher Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(assetbatcher stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(assetbatcher toolkitutil)
TARGET_LINK_LIBRARIES(assetbatcher toolkitutil)
