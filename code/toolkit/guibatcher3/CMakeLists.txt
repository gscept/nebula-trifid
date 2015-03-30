#-------------------------------------------------------------------------------
# Toolkit
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/application
                    ${Nebula3_SOURCE_DIR}/code/addons									
                    ${Nebula3_SOURCE_DIR}/code/application/basegamefeature
                    ${Nebula3_SOURCE_DIR}/code/application/corefeature
                    ${Nebula3_SOURCE_DIR}/code/application/graphicsfeature
                    ${Nebula3_SOURCE_DIR}/code/application/physicsfeature
                    ${Nebula3_SOURCE_DIR}/code/extlibs									
                    ${Nebula3_SOURCE_DIR}/code/foundation
                    ${Nebula3_SOURCE_DIR}/code/render
                    ${Nebula3_SOURCE_DIR}/code/sui
                    ${Nebula3_SOURCE_DIR}/code/tools		
                    ${Nebula3_SOURCE_DIR}/code/toolkit
                    ${Nebula3_SOURCE_DIR}/code/toolkit/toolkitutil
                    ${Nebula3_SOURCE_DIR}/code
)

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("guibatcher3" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${SRCSSPLITTER} ${PCH_files})

ADD_EXECUTABLE(guibatcher3 ${ALLSRCS})
SET_TARGET_PROPERTIES(guibatcher3 PROPERTIES FOLDER "N3SDK/Toolkit")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(guibatcher3)
N_SET_EXE_SUBSYSTEM(guibatcher3 Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(guibatcher3 stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(guibatcher3
	foundation
	db
	attr
	sqlite
	zlib
	toolkitutil
	tinyxml
	base64
	)
TARGET_LINK_LIBRARIES(guibatcher3 	
	db
	attr	
	toolkitutil	
	foundation
	foundation_NIDLS
	tinyxml
	sqlite
	base64
    zlib        
	StackWalker
	${N_LIBS}
	${QT_LIBS} ${QT_LIBRARIES}
	${NEBULA_TOOLKIT_LIBS}
)
