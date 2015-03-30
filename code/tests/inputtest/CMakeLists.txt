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
SOURCE_GROUP("inputtest" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${PCH_files})

ADD_EXECUTABLE(inputtest ${ALLSRCS})
SET_TARGET_PROPERTIES(inputtest PROPERTIES FOLDER "N3SDK/Tests")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(inputtest)
N_SET_EXE_SUBSYSTEM(inputtest Windows)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(inputtest stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(inputtest
	foundation
	zlib
	tinyxml
	${PHYSICS_DEPENDENCIES}
	render
	render_visibilityboxsystemjob render_visibilityquadtreejob
	render_animjobsamplemix render_animjobsample render_particlejob render_charevalskeletonjob
	foundation framecapture sui
	)
TARGET_LINK_LIBRARIES(inputtest 
	foundation
	zlib
	render
	${PHYSICS_DEPENDENCIES}
	render_visibilityboxsystemjob render_visibilityquadtreejob
	render_animjobsamplemix render_animjobsample render_particlejob render_charevalskeletonjob
	foundation framecapture
	tinyxml sui
	${N_LIBS}
)
