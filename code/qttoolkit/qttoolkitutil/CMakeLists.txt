#-------------------------------------------------------------------------------
# qttoolkitutil
#-------------------------------------------------------------------------------

PROJECT(qttoolkitutil)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
					${Nebula3_SOURCE_DIR}/code
					${Nebula3_SOURCE_DIR}/code/foundation
					${Nebula3_SOURCE_DIR}/code/foundation/net
					${Nebula3_SOURCE_DIR}/code/render
					${Nebula3_SOURCE_DIR}/code/toolkit
					${Nebula3_SOURCE_DIR}/code/toolkit/toolkitutil
					${Nebula3_SOURCE_DIR}/code/qttoolkit/qttoolkitutil/code)


FILE(GLOB SRC "*.cc") 
FILE(GLOB HEAD "*.h")
SOURCE_GROUP("qttoolkitutil" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "style/*.cc") 
FILE(GLOB HEAD "style/*.h")
SOURCE_GROUP("qttoolkitutil\\style" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})


SET(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${CODE} ${PCH_files})
ADD_LIBRARY(qttoolkitutil STATIC ${ALLSRCS})
SET_TARGET_PROPERTIES(qttoolkitutil PROPERTIES FOLDER "N3SDK/QtToolkit")
N_SET_COMPILER_DEFAULTS()		

