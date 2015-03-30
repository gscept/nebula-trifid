#-------------------------------------------------------------------------------
# Havok test application for loading an asset as a physics body
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
					${Nebula3_SOURCE_DIR}
					${Nebula3_SOURCE_DIR}/foundation
					${Nebula3_SOURCE_DIR}/render
					${Nebula3_SOURCE_DIR}/application							
					${Nebula3_SOURCE_DIR}/application/basegamefeature
					${Nebula3_SOURCE_DIR}/application/corefeature
					${Nebula3_SOURCE_DIR}/application/graphicsfeature
					${Nebula3_SOURCE_DIR}/application/physicsfeature
					${Nebula3_SOURCE_DIR}/addons		
					${Nebula3_SOURCE_DIR}/extlibs
					${Nebula3_SOURCE_DIR}/sui
					${Nebula3_SOURCE_DIR}/tools		
					${Nebula3_SOURCE_DIR}/toolkit
					${Nebula3_SOURCE_DIR}/tests/
					${Nebula3_SOURCE_DIR}
					
					${Physics_SOURCE_DIR}
					)
link_directories(${N_LIBDIRS})

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("havok_load_asset" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${PCH_files})

ADD_EXECUTABLE(havok_load_asset ${ALLSRCS})
SET_TARGET_PROPERTIES(havok_load_asset PROPERTIES FOLDER "N3SDK/Tests")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(havok_load_asset)
N_SET_EXE_SUBSYSTEM(havok_load_asset Windows)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(havok_load_asset stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(havok_load_asset
	${NEBULA_DEFAULT_LIBS}
	foundation
	zlib
	tinyxml
	)
TARGET_LINK_LIBRARIES(havok_load_asset 
	${NEBULA_DEFAULT_LIBS}
	foundation
	zlib
	tinyxml
	render
	sui audio2 remote attr vibration tinyxml zlib sqlite db
		posteffect render_visibilityboxsystemjob render_visibilityquadtreejob
		render_animjobsamplemix render_animjobsample render_particlejob render_charevalskeletonjob
	${N_LIBS}
	${HAVOK_LIBS}
)
