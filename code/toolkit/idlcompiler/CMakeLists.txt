#-------------------------------------------------------------------------------
# Tools
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(				
									${Nebula3_SOURCE_DIR}/code/foundation																		
									${Nebula3_SOURCE_DIR}/code/tools	
									${Nebula3_SOURCE_DIR}/code/toolkit
									${Nebula3_SOURCE_DIR}/code/extlibs
									${Nebula3_SOURCE_DIR}/code
					)

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("idlc" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})
ADD_EXECUTABLE(idlc ${SRCS} ${PCH_files})
SET_TARGET_PROPERTIES(idlc PROPERTIES FOLDER "N3SDK/Toolkit")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(idlc)
N_SET_EXE_SUBSYSTEM(idlc Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(idlc stdneb.h stdneb.cc ../)
ENDIF()

ADD_DEPENDENCIES(idlc
	foundation
	zlib
	tinyxml
	base64
	toolkitutil
	)
TARGET_LINK_LIBRARIES(idlc 
	foundation
	zlib
	toolkitutil
	tinyxml
	base64
	StackWalker
	${N_LIBS}
)

IF(N_BUILD_FIRSTTIME)
	SET_TARGET_PROPERTIES(idlc PROPERTIES RELEASE_POSTFIX ".toolkit")
ENDIF()
