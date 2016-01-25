#-------------------------------------------------------------------------------
# Level Editor 2.0
#-------------------------------------------------------------------------------

PROJECT(leveleditor2)

SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTNETWORK TRUE)

		
FILE(GLOB SRC "code/*.cc") 
FILE(GLOB HEAD "code/*.h")
SOURCE_GROUP("leveleditor2" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/widgets/*.cc") 
FILE(GLOB HEAD "code/widgets/*.h")
SOURCE_GROUP("leveleditor2\\widgets" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/properties/*.cc") 
FILE(GLOB HEAD "code/properties/*.h")
SOURCE_GROUP("leveleditor2\\properties" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/managers/*.cc") 
FILE(GLOB HEAD "code/managers/*.h")
SOURCE_GROUP("leveleditor2\\managers" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/actions/*.cc") 
FILE(GLOB HEAD "code/actions/*.h")
SOURCE_GROUP("leveleditor2\\actions" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/lighting/*.cc") 
FILE(GLOB HEAD "code/lighting/*.h")
SOURCE_GROUP("leveleditor2\\lighting" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/layers/*.cc") 
FILE(GLOB HEAD "code/layers/*.h")
SOURCE_GROUP("leveleditor2\\layers" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/entityutils/*.cc") 
FILE(GLOB HEAD "code/entityutils/*.h")
SOURCE_GROUP("leveleditor2\\entityutils" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

SET(P_NIDL_FILES
	leveleditor2protocol.nidl
)

		

FILE(GLOB FORMS "ui/*.ui")

FILE(GLOB RESOURCES ${Nebula3_SOURCE_DIR}/code/qttoolkit/*.qrc)

SET(QTMOCFILES	"code/widgets/leveleditor2window.h"
				"code/leveleditor2app.h"
				"code/widgets/createentitydialoghandler.h"
				"code/widgets/blueprinthandler.h"
				"code/widgets/entityhandler.h"
				"code/widgets/levelpropertieshandler.h"
				"code/widgets/audiodialoghandler.h"
				"code/widgets/uidialoghandler.h"
				"code/lighting/lightprobemanager.h"
				"code/layers/layerhandler.h")

SET(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})
PCH_QT4_WRAP_CPP(leveleditor2_HEADERS_MOC ${QTMOCFILES})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
QT4_WRAP_UI(leveleditor2_FORMS_HEADERS ${FORMS})
NONPCH_QT4_ADD_RESOURCES(leveleditor2_RESOURCES_RCC ${RESOURCES})

foreach (it ${leveleditor2_RESOURCES_RCC})
#Message(${it})
#	SET_SOURCE_FILES_PROPERTIES(${it} PROPERTIES COMPILE_FLAGS /Y-)
endforeach()

LIST(APPEND ALLSRCS ${CODE} ${PCH_files} ${leveleditor2_HEADERS_MOC} ${leveleditor2_FORMS_HEADERS} ${leveleditor2_RESOURCES_RCC} code/leveleditor.rc)

N_WRAP_ADD_NIDL_FILES(ALLSRCS ${P_NIDL_FILES})

#STRING(REPLACE "/MTd" "/MDd" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
#STRING(REPLACE "/MT" "/MD" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
REMOVE_DEFINITIONS(-DQT_DEBUG)
#STRING(REPLACE "/MANIFEST:NO" "" CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

IF(MSVC)
ADD_DEFINITIONS(/D_HAS_EXCEPTIONS=1)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} /EHsc)
ENDIF()

ADD_EXECUTABLE(leveleditor2 ${ALLSRCS})

TARGET_INCLUDE_DIRECTORIES(leveleditor2 PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${Nebula3_SOURCE_DIR}/code/qttoolkit/leveleditor2/code)

N_SET_COMPILER_DEFAULTS()
N_SET_QT_PROPERTIES(leveleditor2 "N3SDK/QtToolkit")
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(leveleditor2 stdneb.h stdneb.cc ../)
ENDIF()
		
N_SET_EXE_SUBSYSTEM(leveleditor2 Windows)

ADD_DEPENDENCIES(leveleditor2	
	application
	grid
	silhouette
	qtfeature 
	CubeMapGen
	toolkitutil	
	libqimg
	attributecontrollerwidget
	editorfeatures
	remoteinterface	
	qttools
	posteffectcontroller
	globalprobecontroller
	splash
	dynui
	navigation	
)

TARGET_LINK_LIBRARIES(leveleditor2 
	grid
	silhouette
	qtfeature
	libqimg
	attributecontrollerwidget
	remoteinterface	
	qttools
	editorfeatures	
	toolkitutil 
	CubeMapGen
	qttools
	posteffectcontroller
	globalprobecontroller
	splash	
	navigation	
	dynui
	${QT_WIN_LIBS}	
	${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY}	
)
