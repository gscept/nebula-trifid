#-------------------------------------------------------------------------------
# Toolkit
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
									${Nebula3_SOURCE_DIR}/toolkit
									${Nebula3_SOURCE_DIR}/tests/
									${Nebula3_SOURCE_DIR}
					)
link_directories(${N_LIBDIRS})

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("spumathtest" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${PCH_files})

ADD_EXECUTABLE(spumathtest ${ALLSRCS})
SET_TARGET_PROPERTIES(spumathtest PROPERTIES FOLDER "N3SDK/Tests")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(spumathtest)
N_SET_EXE_SUBSYSTEM(spumathtest Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(spumathtest stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(spumathtest
	foundation
	zlib
	tinyxml
	)
TARGET_LINK_LIBRARIES(spumathtest 
	foundation
	zlib
	tinyxml
	${N_EXTLIBS_WIN32_LIBS}
	${N_EXTRA_WIN32_LIBS}
	${N_DIRECTX_LIBS}
)
