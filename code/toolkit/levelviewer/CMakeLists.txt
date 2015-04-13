#-------------------------------------------------------------------------------
# Toolkit
#-------------------------------------------------------------------------------

FILE(GLOB SRC "gamestates/*.cc")
FILE(GLOB HEAD "gamestates/*.h")
LIST(APPEND SRCSGAMESTATES ${SRC} ${HEAD})
SOURCE_GROUP("levelviewer\\gamestates" FILES ${SRCSGAMESTATES})

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCSBASE ${SRC} ${HEAD})
SOURCE_GROUP("levelviewer" FILES ${SRCSBASE})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCSBASE} ${SRCSGAMESTATES} ${PCH_files} levelviewer.rc)

ADD_EXECUTABLE(levelviewer ${ALLSRCS})
SET_TARGET_PROPERTIES(levelviewer PROPERTIES FOLDER "N3SDK/Toolkit")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(levelviewer)
N_SET_EXE_SUBSYSTEM(levelviewer Windows)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(levelviewer stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(levelviewer
	application
	remoteinterface
	dynui
	)
TARGET_LINK_LIBRARIES(levelviewer 	
	remoteinterface
	application	
	dynui
)
