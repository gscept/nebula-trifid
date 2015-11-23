#-------------------------------------------------------------------------------
# resourcebrowser
#-------------------------------------------------------------------------------

PROJECT(resourcebrowser)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
					${Nebula3_SOURCE_DIR}/code
					${Nebula3_SOURCE_DIR}/code/foundation
					${Nebula3_SOURCE_DIR}/code/application
					${Nebula3_SOURCE_DIR}/code/extlibs					
					${Nebula3_SOURCE_DIR}/code/render
					${Nebula3_SOURCE_DIR}/code/toolkit
					${Nebula3_SOURCE_DIR}/code/toolkit/toolkitutil
					${Nebula3_SOURCE_DIR}/code/qttoolkit/qtaddons/resourcebrowser/code)


FILE(GLOB SRC "code/*.cc") 
FILE(GLOB HEAD "code/*.h")
FILE(GLOB FORMS "ui/*.ui")
SOURCE_GROUP("resourcebrowser" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

SET(QTMOCFILES 
	"code/assetbrowser.h"
	"code/tiledgraphicsview.h"
	"code/tiledgraphicsitem.h"
	"code/tiledtextureitem.h"
    "code/tiledsurfaceitem.h"
    "code/tiledmodelitem.h"
	"code/tileddirectoryitem.h"
	"code/imageloaderthread.h")
PCH_QT4_WRAP_CPP(resourcebrowser_HEADERS_MOC ${QTMOCFILES})
QT4_WRAP_UI(resourcebrowser_FORMS_HEADERS ${FORMS})

SET(PCH_files	
    ../../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})


LIST(APPEND ALLSRCS ${CODE} ${PCH_files} ${resourcebrowser_HEADERS_MOC} ${resourcebrowser_FORMS_HEADERS})
ADD_LIBRARY(resourcebrowser STATIC ${ALLSRCS})
SET_TARGET_PROPERTIES(resourcebrowser PROPERTIES FOLDER "N3SDK/QtAddons")
N_SET_COMPILER_DEFAULTS()		

