#-------------------------------------------------------------------------------
# fbxbatcher
#-------------------------------------------------------------------------------

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("fbxbatcher3" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${SRCSSPLITTER} ${PCH_files})

ADD_EXECUTABLE(fbxbatcher3 ${ALLSRCS})
SET_TARGET_PROPERTIES(fbxbatcher3 PROPERTIES FOLDER "N3SDK/Toolkit")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(fbxbatcher3)
N_SET_EXE_SUBSYSTEM(fbxbatcher3 Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(fbxbatcher3 stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(fbxbatcher3 toolkitutil	)
TARGET_LINK_LIBRARIES(fbxbatcher3 toolkitutil )
