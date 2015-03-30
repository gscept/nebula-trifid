#-------------------------------------------------------------------------------
# Toolkit
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/application
                    ${Nebula3_SOURCE_DIR}/code/addons
                    ${Nebula3_SOURCE_DIR}/code/application/basegamefeature
                    ${Nebula3_SOURCE_DIR}/code/application/corefeature
                    ${Nebula3_SOURCE_DIR}/code/application/graphicsfeature
                    ${Nebula3_SOURCE_DIR}/code/application/physicsfeature
                    ${Nebula3_SOURCE_DIR}/code/extlibs
                    ${Nebula3_SOURCE_DIR}/code/foundation
                    ${Nebula3_SOURCE_DIR}/code/render
                    ${Nebula3_SOURCE_DIR}/code/sui
                    ${Nebula3_SOURCE_DIR}/code/tools
                    ${Nebula3_SOURCE_DIR}/code/toolkit
                    ${Nebula3_SOURCE_DIR}/code/tests/
                    ${Nebula3_SOURCE_DIR}/code
)

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
	${NEBULA_DEFAULT_LIBS}	
	${ROCKET}
	remoteinterface
	)
TARGET_LINK_LIBRARIES(levelviewer 	
	remoteinterface
	remoteinterface_NIDLS
	scripting_NIDLS
	scripting
	${N_LIBS}
	${ROCKET}
	${NEBULA_DEFAULT_LIBS}	
	${NEBULA_DEFAULT_LIBS}
	db
	sqlite
	attr
	scripting_NIDLS
	scripting
	lua
)
