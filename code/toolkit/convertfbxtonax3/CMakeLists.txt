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
									${Nebula3_SOURCE_DIR}/extlibs/fbx/include
									${Nebula3_SOURCE_DIR}/foundation
									${Nebula3_SOURCE_DIR}/render
									${Nebula3_SOURCE_DIR}/sui
									${Nebula3_SOURCE_DIR}/tools		
									${Nebula3_SOURCE_DIR}/toolkit
									${Nebula3_SOURCE_DIR}
					)
link_directories(${N_LIBDIRS}  ${Nebula3_SOURCE_DIR}/../lib/fbx)

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("convertfbxtonax3" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${PCH_files})

ADD_EXECUTABLE(convertfbxtonax3 ${ALLSRCS})
SET_TARGET_PROPERTIES(convertfbxtonax3 PROPERTIES FOLDER "N3SDK/Toolkit")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(convertfbxtonax3)
N_SET_EXE_SUBSYSTEM(convertfbxtonax3 Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(convertfbxtonax3 stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(convertfbxtonax3
	foundation
	zlib
	toolkitutil
	tinyxml
	)
TARGET_LINK_LIBRARIES(convertfbxtonax3 
	foundation
	zlib
	toolkitutil
	tinyxml	
	${N_EXTLIBS_WIN32_LIBS}
	${N_EXTRA_WIN32_LIBS}
	${N_DIRECTX_LIBS}
	debug
	${FBX_LIBS_DEBUG}
	optimized
	${FBX_LIBS_RELEASE}
)
