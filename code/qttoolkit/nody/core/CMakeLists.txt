#-------------------------------------------------------------------------------
# nody lib
#-------------------------------------------------------------------------------

PROJECT(nody)

SET(QT_USE_QTOPENGL TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTNETWORK TRUE)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
                    ${Nebula3_SOURCE_DIR}/code/addons
                    )
LINK_DIRECTORIES(${N_LIBDIRS})

FILE(GLOB SRC "*.cc") 
FILE(GLOB HEAD "*.h")
SOURCE_GROUP("nody" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "generation/*.cc") 
FILE(GLOB HEAD "generation/*.h")
SOURCE_GROUP("nody\\generation" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "link/*.cc")
FILE(GLOB HEAD "link/*.h")
SOURCE_GROUP("nody\\link" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "link/graphics/*.cc")
FILE(GLOB HEAD "link/graphics/*.h")
SOURCE_GROUP("nody\\link\\graphics" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "node/*.cc") 
FILE(GLOB HEAD "node/*.h")
SOURCE_GROUP("nody\\node" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "node/graphics/*.cc") 
FILE(GLOB HEAD "node/graphics/*.h")
SOURCE_GROUP("nody\\node\\graphics" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "project/*.cc") 
FILE(GLOB HEAD "project/*.h")
SOURCE_GROUP("nody\\project" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "scene/*.cc") 
FILE(GLOB HEAD "scene/*.h")
SOURCE_GROUP("nody\\scene" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "scene/graphics/*.cc") 
FILE(GLOB HEAD "scene/graphics/*.h")
SOURCE_GROUP("nody\\scene\\graphics" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "variable/*.cc")
FILE(GLOB HEAD "variable/*.h")
SOURCE_GROUP("nody\\variable" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "variable/graphics/*.cc")
FILE(GLOB HEAD "variable/graphics/*.h")
SOURCE_GROUP("nody\\variable\\graphics" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "variation/*.cc") 
FILE(GLOB HEAD "variation/*.h")
SOURCE_GROUP("nody\\variation" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "variation/graphics/*.cc") 
FILE(GLOB HEAD "variation/graphics/*.h")
SOURCE_GROUP("nody\\variation\\graphics" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "simulation/*.cc") 
FILE(GLOB HEAD "simulation/*.h")
SOURCE_GROUP("nody\\simulation" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})
				
SET(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

SET(QTMOCFILES 
                "node/graphics/nodegraphics.h"
				"scene/graphics/nodegraphicsscene.h"
				"scene/graphics/nodegraphicssceneview.h"
				"variation/graphics/variationlistwidget.h")

PCH_QT4_WRAP_CPP(nody_HEADERS_MOC ${QTMOCFILES})
SOURCE_GROUP("MOC" FILES ${nody_HEADERS_MOC})
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})


LIST(APPEND ALLSRCS ${CODE} ${PCH_files} ${nody_HEADERS_MOC})
ADD_LIBRARY(nody STATIC ${ALLSRCS})

N_SET_COMPILER_DEFAULTS()
N_SET_QT_PROPERTIES(nody ${NODY_FOLDER})
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(nody stdneb.h stdneb.cc ../)
ENDIF()


