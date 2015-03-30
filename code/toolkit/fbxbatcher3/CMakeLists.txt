#-------------------------------------------------------------------------------
# Toolkit
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(				${Nebula3_SOURCE_DIR}/code/application
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
									${Nebula3_SOURCE_DIR}/code/toolkit/toolkitutil
									${Nebula3_SOURCE_DIR}/code
					)

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
ADD_DEPENDENCIES(fbxbatcher3
	foundation
	zlib
	toolkitutil
	tinyxml
	render
	${PHYSICS_DEPENDENCIES}
	)
TARGET_LINK_LIBRARIES(fbxbatcher3 
	toolkitutil
    ${NEBULA_DEFAULT_LIBS}
	${PHYSICS_DEPENDENCIES}
	${NEBULA_TOOLKIT_LIBS}	
#	${N_LIBS}
)
