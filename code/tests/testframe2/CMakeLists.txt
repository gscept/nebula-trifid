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
									${Nebula3_SOURCE_DIR}/code/tests/
									${Nebula3_SOURCE_DIR}/code
					)


FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("testframe2" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${PCH_files})

ADD_EXECUTABLE(testframe2 ${ALLSRCS})
SET_TARGET_PROPERTIES(testframe2 PROPERTIES FOLDER "N3SDK/Tests")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(testframe2)
N_SET_EXE_SUBSYSTEM(testframe2 Windows)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(testframe2 stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(testframe2
	foundation
	foundation_NIDLS
	render
	render_NIDLS
	zlib
	testbase
	lua
	tinyxml
	)
TARGET_LINK_LIBRARIES(testframe2 
	foundation
	foundation_NIDLS
	render
	render_NIDLS
	zlib
	testbase
	tinyxml
	lua
	
	${N_LIBS}
)
