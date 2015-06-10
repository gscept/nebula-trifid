#-------------------------------------------------------------------------------
# Toolkit
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(    ${Nebula3_SOURCE_DIR}/code/application
                        ${Nebula3_SOURCE_DIR}/code/addons									
                        ${Nebula3_SOURCE_DIR}/code/application/basegamefeature
                        ${Nebula3_SOURCE_DIR}/code/application/corefeature
                        ${Nebula3_SOURCE_DIR}/code/application/graphicsfeature
                        ${Nebula3_SOURCE_DIR}/code/application/physicsfeature
                        ${Nebula3_SOURCE_DIR}/code/extlibs
                        ${Nebula3_SOURCE_DIR}/code/extlibs/ode
                        ${Nebula3_SOURCE_DIR}/code/foundation
                        ${Nebula3_SOURCE_DIR}/code/render
                        ${Nebula3_SOURCE_DIR}/code/sui
                        ${Nebula3_SOURCE_DIR}/code/tools		
                        ${Nebula3_SOURCE_DIR}/code/toolkit
                        ${Nebula3_SOURCE_DIR}/code/tests/
                        ${Nebula3_SOURCE_DIR}
					)
link_directories(${N_LIBDIRS})

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("testrender" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${PCH_files})

ADD_EXECUTABLE(testrender ${ALLSRCS})
SET_TARGET_PROPERTIES(testrender PROPERTIES FOLDER "N3SDK/Tests")

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(testrender)
N_SET_EXE_SUBSYSTEM(testrender Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(testrender stdneb.h stdneb.cc ../)
ENDIF()
ADD_DEPENDENCIES(testrender render dynui framecapture testbase)

TARGET_LINK_LIBRARIES(testrender render attr dynui framecapture testbase)            
