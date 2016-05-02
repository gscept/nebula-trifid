#-------------------------------------------------------------------------------
# shady
#-------------------------------------------------------------------------------

PROJECT(shady)

SET(QT_USE_QTOPENGL TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTNETWORK TRUE)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}			
                    ${Nebula3_SOURCE_DIR}/code/extlibs		
                    ${Nebula3_SOURCE_DIR}/code/addons
                    ${Nebula3_SOURCE_DIR}/code/toolkit
					code)
					
LINK_DIRECTORIES(${N_LIBDIRS})

OPTION(SHADY_APPLICATION "Compiles Shady as a stand-alone application" OFF)

FILE(GLOB SRC "code/*.cc") 
FILE(GLOB HEAD "code/*.h")
SOURCE_GROUP("shady" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/variation/*.cc") 
FILE(GLOB HEAD "code/variation/*.h")
SOURCE_GROUP("shady\\variation" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/variable/*.cc") 
FILE(GLOB HEAD "code/variable/*.h")
SOURCE_GROUP("shady\\variable" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/node/*.cc") 
FILE(GLOB HEAD "code/node/*.h")
SOURCE_GROUP("shady\\node" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/project/*.cc") 
FILE(GLOB HEAD "code/project/*.h")
SOURCE_GROUP("shady\\project" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/generation/*.cc") 
FILE(GLOB HEAD "code/generation/*.h")
SOURCE_GROUP("shady\\generation" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/node/graphics/*.cc") 
FILE(GLOB HEAD "code/node/graphics/*.h")
SOURCE_GROUP("shady\\node\\graphics" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/simulation/*.cc") 
FILE(GLOB HEAD "code/simulation/*.h")
SOURCE_GROUP("shady\\simulation" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB FORMS "ui/*.ui")
FILE(GLOB RESOURCES ${QTTOOLS}/ui/*.qrc)

SET(QTMOCFILES  
    "code/node/graphics/shadynodegraphics.h"
	"code/shadywindow.h"
	"code/materialwindow.h")

SET(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

N_WRAP_ADD_PROJ_NIDL_FILES(shady CODE code/simulation/simulationcommands.nidl)

PCH_QT4_WRAP_CPP(shady_HEADERS_MOC ${QTMOCFILES})
SOURCE_GROUP("MOC" FILES ${shady_HEADERS_MOC})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
QT4_WRAP_UI(shady_FORMS_HEADERS ${FORMS})
NONPCH_QT4_ADD_RESOURCES(shady_RESOURCES_RCC ${RESOURCES})


LIST(APPEND ALLSRCS ${CODE} ${PCH_files} ${shady_HEADERS_MOC} ${shady_FORMS_HEADERS} ${shady_RESOURCES_RCC})
REMOVE_DEFINITIONS(-DQT_DEBUG)
#STRING(REPLACE "/MANIFEST:NO" "" CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

IF(SHADY_APPLICATION)
    ADD_EXECUTABLE(shady ${ALLSRCS})
    N_SET_EXE_LINKER_DEFAULTS(shady)
    N_SET_EXE_SUBSYSTEM(shady Windows)

    ADD_DEPENDENCIES(shady        
        libqimg
	    ${NEBULA_DEFAULT_LIBS}
	    nody
	    toolkitutil
    )
    TARGET_LINK_LIBRARIES(shady        
        libqimg
        ${NEBULA_DEFAULT_LIBS}
	    ${NEBULA_TOOLKIT_LIBS}
	    nody 
	    toolkitutil
	    ${QT_WIN_LIBS}
	    ${N_EXTRA_LIBS}
	    ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY}
	    d3dx11.lib 
	    d3dcompiler.lib 
	    opengl32.lib
    )

    N_SET_EXE_LINKER_DEFAULTS(shady)
    N_SET_EXE_SUBSYSTEM(shady Windows)

    #build as library
ELSE()
    # remove main files from CMake
    FILE(GLOB MAIN "code/main.cc" "code/shadyapp.h" "code/shadyapp.cc")
    LIST(REMOVE_ITEM ALLSRCS ${MAIN})
    ADD_LIBRARY(shady ${ALLSRCS})
ENDIF()

N_SET_COMPILER_DEFAULTS()
N_SET_QT_PROPERTIES(shady ${NODY_FOLDER})
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(shady stdneb.h stdneb.cc ../)
ENDIF()