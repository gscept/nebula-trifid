#-------------------------------------------------------------------------------
# Tools
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(				${Nebula3_SOURCE_DIR}/code/application
									${Nebula3_SOURCE_DIR}/code/addons									
									${Nebula3_SOURCE_DIR}/code/application/basegamefeature
									${Nebula3_SOURCE_DIR}/code/application/corefeature
									${Nebula3_SOURCE_DIR}/code/application/graphicsfeature
									${Nebula3_SOURCE_DIR}/code/application/physicsfeature
									${Nebula3_SOURCE_DIR}/code/extlibs
									${Nebula3_SOURCE_DIR}/code/extlibs/ode
									${Nebula3_SOURCE_DIR}/code/foundation
									${Nebula3_SOURCE_DIR}/code/render
									${Nebula3_SOURCE_DIR}/code/sui
									${Nebula3_SOURCE_DIR}/code/tools		
									${Nebula3_SOURCE_DIR}/code
					)


FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("nsh3" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${PCH_files})

ADD_EXECUTABLE(nsh3 ${ALLSRCS})
SET_TARGET_PROPERTIES(nsh3 PROPERTIES FOLDER "N3SDK/Toolkit")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(nsh3)
N_SET_EXE_SUBSYSTEM(nsh3 Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(nsh3 stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(nsh3
	foundation
	foundation_NIDLS
	zlib
	tinyxml
	scripting
	scripting_NIDLS
	lua
	)
TARGET_LINK_LIBRARIES(nsh3 
	foundation
	foundation_NIDLS
	zlib
	tinyxml
	scripting
	scripting_NIDLS
	lua
	${N_LIBS}
)