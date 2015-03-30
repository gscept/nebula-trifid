#-------------------------------------------------------------------------------
# Level Editor 2.0
#-------------------------------------------------------------------------------

PROJECT(leveleditor2)

SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTNETWORK TRUE)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
					${Nebula3_SOURCE_DIR}/code
					${Nebula3_SOURCE_DIR}/code/foundation
					${Nebula3_SOURCE_DIR}/code/render
					${Nebula3_SOURCE_DIR}/code/application				
					${Nebula3_SOURCE_DIR}/code/application/basegamefeature
					${Nebula3_SOURCE_DIR}/code/application/corefeature
					${Nebula3_SOURCE_DIR}/code/application/graphicsfeature
					${Nebula3_SOURCE_DIR}/code/application/physicsfeature
					${Nebula3_SOURCE_DIR}/code/addons
					${Nebula3_SOURCE_DIR}/code/extlibs
					${Nebula3_SOURCE_DIR}/code/toolkit
					${Nebula3_SOURCE_DIR}/code/toolkit/toolkitutil
					${Nebula3_SOURCE_DIR}/code/qttoolkit
					${Nebula3_SOURCE_DIR}/code/qttoolkit/qtaddons/posteffectcontroller/code
					${Nebula3_SOURCE_DIR}/code/qttoolkit/qtaddons/attributecontrollerwidget/code
					${Nebula3_SOURCE_DIR}/code/qttoolkit/qttools/code
					${Nebula3_SOURCE_DIR}/code/qttoolkit/leveleditor2/code
					${Nebula3_SOURCE_DIR}/code/extlibs/recastnavigation/Recast/Include
					${Nebula3_SOURCE_DIR}/code/extlibs/recastnavigation/Detour/Include
					${Nebula3_SOURCE_DIR}/code/qttoolkit/qtaddons/attributecontrollerwidget/code					
					${Physics_SOURCE_DIR}
					)
					
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
	
N_WRAP_NIDL_FILES(leveleditor2_NIDLS ${P_NIDL_FILES})
		

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

#STRING(REPLACE "/MTd" "/MDd" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
#STRING(REPLACE "/MT" "/MD" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
REMOVE_DEFINITIONS(-DQT_DEBUG)
STRING(REPLACE "/MANIFEST:NO" "" CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

ADD_EXECUTABLE(leveleditor2 ${ALLSRCS})


N_SET_COMPILER_DEFAULTS()
N_SET_QT_PROPERTIES(leveleditor2 "N3SDK/QtToolkit")
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(leveleditor2 stdneb.h stdneb.cc ../)
ENDIF()
		
N_SET_EXE_SUBSYSTEM(leveleditor2 Windows)

ADD_DEPENDENCIES(leveleditor2
	${NEBULA_DEFAULT_LIBS}
	grid
	silhouette
	qtfeature 
	CubeMapGen
	toolkitutil	
	libqimg
	attributecontrollerwidget
	editorfeatures
	remoteinterface
	remoteinterface_NIDLS
	qttoolkitutil
	posteffectcontroller
	splash
	navigation
	Recast
	Detour	
	DebugUtils	
	leveleditor2_NIDLS
)
ADD_DEPENDENCIES(leveleditor2_NIDLS navigation_NIDLS)

TARGET_LINK_LIBRARIES(leveleditor2 
	grid
	silhouette
	qtfeature
	libqimg
	attributecontrollerwidget
	remoteinterface
	remoteinterface_NIDLS
	qttoolkitutil
	editorfeatures
	CubeMapGen
	toolkitutil 
	posteffectcontroller
	splash
	Recast
	leveleditor2_NIDLS
	${NEBULA_DEFAULT_LIBS}
	navigation
	DebugUtils
	Detour
    imgui
    
	${N_FBX_LIBS}
	${QT_WIN_LIBS}
	${N_LIBS}
	${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY}	
)
