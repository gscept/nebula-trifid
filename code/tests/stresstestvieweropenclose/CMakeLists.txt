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
SOURCE_GROUP("stresstestvieweropenclose" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${PCH_files})

ADD_EXECUTABLE(stresstestvieweropenclose ${ALLSRCS})
SET_TARGET_PROPERTIES(stresstestvieweropenclose PROPERTIES FOLDER "N3SDK/Tests")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(stresstestvieweropenclose)
N_SET_EXE_SUBSYSTEM(stresstestvieweropenclose Windows)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(stresstestvieweropenclose stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(stresstestvieweropenclose
	foundation
	zlib
	tinyxml
	sui audio2 remote attr vibration tinyxml zlib sqlite db posteffect render_visibilityboxsystemjob render_visibilityquadtreejob
	render_animjobsamplemix render_animjobsample render_particlejob render_charevalskeletonjob
	foundation framecapture
	${PHYSICS_DEPENDENCIES}
	render
	application
	)
TARGET_LINK_LIBRARIES(stresstestvieweropenclose 
	foundation
	zlib
	tinyxml
	sui audio2 remote attr vibration tinyxml zlib sqlite db posteffect render_visibilityboxsystemjob render_visibilityquadtreejob
	render_animjobsamplemix render_animjobsample render_particlejob render_charevalskeletonjob
	foundation framecapture
	${PHYSICS_DEPENDENCIES}
	render
	application
	${N_LIBS}
)
