#-------------------------------------------------------------------------------
# Tools
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(				${Nebula3_SOURCE_DIR}/application
									${Nebula3_SOURCE_DIR}/addons									
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
									${Nebula3_SOURCE_DIR}
					)
link_directories(${N_LIBDIRS})

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("sm5compiler" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${PCH_files})

ADD_EXECUTABLE(sm5compiler ${ALLSRCS})
SET_TARGET_PROPERTIES(sm5compiler PROPERTIES FOLDER "N3SDK/Toolkit")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(sm5compiler)
N_SET_EXE_SUBSYSTEM(sm5compiler Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(sm5compiler stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(sm5compiler
	foundation
	zlib
	tinyxml
	)
TARGET_LINK_LIBRARIES(sm5compiler 
	foundation
	zlib
	tinyxml
	${N_EXTLIBS_WIN32_LIBS}
	${N_EXTRA_WIN32_LIBS}
	${N_DIRECTX_LIBS}
)