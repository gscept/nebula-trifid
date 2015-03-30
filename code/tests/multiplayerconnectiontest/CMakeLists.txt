#-------------------------------------------------------------------------------
# Toolkit
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(				${Nebula3_SOURCE_DIR}/application
									${Nebula3_SOURCE_DIR}/addons									
									${Nebula3_SOURCE_DIR}/addons/network
									${Nebula3_SOURCE_DIR}/application/basegamefeature
									${Nebula3_SOURCE_DIR}/application/corefeature
									${Nebula3_SOURCE_DIR}/application/graphicsfeature
									${Nebula3_SOURCE_DIR}/application/physicsfeature
									${Nebula3_SOURCE_DIR}/extlibs
									${Nebula3_SOURCE_DIR}/extlibs/ode
									${Nebula3_SOURCE_DIR}/foundation
									${Nebula3_SOURCE_DIR}/render
									${Nebula3_SOURCE_DIR}/sui
									${Nebula3_SOURCE_DIR}/tools		
									${Nebula3_SOURCE_DIR}/toolkit
									${Nebula3_SOURCE_DIR}/tests/
									${Nebula3_SOURCE_DIR}
					)
link_directories(${N_LIBDIRS})

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("multiplayerconnectiontest" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${PCH_files})

ADD_EXECUTABLE(multiplayerconnectiontest ${ALLSRCS})
SET_TARGET_PROPERTIES(multiplayerconnectiontest PROPERTIES FOLDER "N3SDK/Tests")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(multiplayerconnectiontest)
N_SET_EXE_SUBSYSTEM(multiplayerconnectiontest Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(multiplayerconnectiontest stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(multiplayerconnectiontest
	foundation
	zlib
	tinyxml
	raknet
	network
	testbase
	)
TARGET_LINK_LIBRARIES(multiplayerconnectiontest 
	foundation
	zlib
	testbase
	tinyxml
	network
	raknet
	ws2_32.lib
	${N_LIBS}
)
