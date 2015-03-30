#-------------------------------------------------------------------------------
# Benchmarkbase
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(				${Nebula3_SOURCE_DIR}/code/code/application
									${Nebula3_SOURCE_DIR}/code/code/addons									
									${Nebula3_SOURCE_DIR}/code/application/basegamefeature
									${Nebula3_SOURCE_DIR}/code/application/corefeature
									${Nebula3_SOURCE_DIR}/code/application/graphicsfeature
									${Nebula3_SOURCE_DIR}/code/application/physicsfeature
									${Nebula3_SOURCE_DIR}/code/extlibs
									${Nebula3_SOURCE_DIR}/code/extlibs/bullet/bullet/src
									${Nebula3_SOURCE_DIR}/code/extlibs/ode
									${Nebula3_SOURCE_DIR}/code/extlibs/fbx
									${Nebula3_SOURCE_DIR}/code/foundation
									${Nebula3_SOURCE_DIR}/code/render
									${Nebula3_SOURCE_DIR}/code/sui									
									${Nebula3_SOURCE_DIR}/code/benchmarks
									${Nebula3_SOURCE_DIR}/code
					)
link_directories(${N_LIBDIRS})

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCST ${SRC} ${HEAD})
SOURCE_GROUP("benchmarkbase" FILES ${SRCST})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCST} ${PCH_files})


add_library(benchmarkbase STATIC ${ALLSRCS} )
SET_TARGET_PROPERTIES(benchmarkbase PROPERTIES FOLDER "N3SDK/Benchmarks")
N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(benchmarkbase stdneb.h stdneb.cc ../)
ENDIF()
