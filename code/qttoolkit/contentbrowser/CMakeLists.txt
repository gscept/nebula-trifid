#-------------------------------------------------------------------------------
# contentbrowser
#-------------------------------------------------------------------------------

PROJECT(contentbrowser)

SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTOPENGL TRUE)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}

					${Nebula3_SOURCE_DIR}/code
					${Nebula3_SOURCE_DIR}/code/foundation
					${Nebula3_SOURCE_DIR}/code/render
					${Nebula3_SOURCE_DIR}/code/application
					${Nebula3_SOURCE_DIR}/code/application/graphicsfeature
					${Nebula3_SOURCE_DIR}/code/addons
					${Nebula3_SOURCE_DIR}/code/extlibs
					${Nebula3_SOURCE_DIR}/code/toolkit
					${Nebula3_SOURCE_DIR}/code/toolkit/toolkitutil
					${Nebula3_SOURCE_DIR}/code/qttoolkit/
					${Nebula3_SOURCE_DIR}/code/qtaddons/miniexporter
					${Nebula3_SOURCE_DIR}/code/qttoolkit/qttools/code
					${Nebula3_SOURCE_DIR}/code/qttoolkit/nody/core
					${Nebula3_SOURCE_DIR}/code/qttoolkit/nody/shady/code
					${Nebula3_SOURCE_DIR}/code/qttoolkit/contentbrowser/code
					${Nebula3_SOURCE_DIR}/code/qttoolkit/qtaddons/posteffectcontroller/code
					${Nebula3_SOURCE_DIR}/code/qttoolkit/qtaddons/miniexporter/code					
					${Physics_SOURCE_DIR}
					)

FILE(GLOB SRC "code/*.cc") 
FILE(GLOB HEAD "code/*.h")
SOURCE_GROUP("contentbrowser" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/previewer/*.cc") 
FILE(GLOB HEAD "code/previewer/*.h")
SOURCE_GROUP("contentbrowser\\previewer" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/modelimporter/*.cc") 
FILE(GLOB HEAD "code/modelimporter/*.h")
SOURCE_GROUP("contentbrowser\\modelimporter" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/lighting/*.cc") 
FILE(GLOB HEAD "code/lighting/*.h")
SOURCE_GROUP("contentbrowser\\lighting" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/modelimporter/fbx/*.cc") 
FILE(GLOB HEAD "code/modelimporter/fbx/*.h")
SOURCE_GROUP("contentbrowser\\modelimporter\\fbx" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/textureimporter/*.cc") 
FILE(GLOB HEAD "code/textureimporter/*.h")
SOURCE_GROUP("contentbrowser\\textureimporter" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/widgets/*.cc")
FILE(GLOB HEAD "code/widgets/*.h")
SOURCE_GROUP("contentbrowser\\widgets" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/widgets/animations/*.cc")
FILE(GLOB HEAD "code/widgets/animations/*.h")
SOURCE_GROUP("contentbrowser\\widgets\\animations" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/widgets/audio/*.cc")
FILE(GLOB HEAD "code/widgets/audio/*.h")
SOURCE_GROUP("contentbrowser\\widgets\\audio" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/widgets/ui/*.cc")
FILE(GLOB HEAD "code/widgets/ui/*.h")
SOURCE_GROUP("contentbrowser\\widgets\\ui" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/widgets/meshes/*.cc")
FILE(GLOB HEAD "code/widgets/meshes/*.h")
SOURCE_GROUP("contentbrowser\\widgets\\meshes" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/widgets/models/*.cc")
FILE(GLOB HEAD "code/widgets/models/*.h")
SOURCE_GROUP("contentbrowser\\widgets\\models" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/widgets/textures/*.cc")
FILE(GLOB HEAD "code/widgets/textures/*.h")
SOURCE_GROUP("contentbrowser\\widgets\\textures" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/widgets/particles/*.cc")
FILE(GLOB HEAD "code/widgets/particles/*.h")
SOURCE_GROUP("contentbrowser\\widgets\\particles" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/materials/*.cc")
FILE(GLOB HEAD "code/materials/*.h")
SOURCE_GROUP("contentbrowser\\materials" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/actions/*.cc")
FILE(GLOB HEAD "code/actions/*.h")
SOURCE_GROUP("contentbrowser\\actions" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB FORMS "ui/*.ui")
FILE(GLOB RESOURCES ${Nebula3_SOURCE_DIR}/code/qttoolkit/*.qrc)

SET(QTMOCFILES  "code/contentbrowserwindow.h" 
				"code/importerwindow.h"
				"code/modelimporter/modelimporterwindow.h" 
				"code/modelimporter/clipcontroller.h"
				"code/modelimporter/modelcontroller.h"
				"code/modelimporter/physicscontroller.h"
				"code/textureimporter/textureimporterwindow.h"
				"code/widgets/basehandler.h"
				"code/widgets/models/modelhandler.h"
				"code/widgets/models/modelnodehandler.h"
				"code/widgets/models/characternodehandler.h"
				"code/widgets/models/particlenodehandler.h"
				"code/widgets/models/trackcontroller.h"
				"code/widgets/textures/texturehandler.h"
				"code/widgets/audio/audiohandler.h"
				"code/widgets/ui/uihandler.h"
				"code/widgets/particles/particlecurvewidget.h"
				"code/widgets/particles/particleenvelopewidget.h"
				"code/lighting/environmentprobewindow.h"
				"code/modelimporter/fbx/nfbxreader.h")
								
SET(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})
PCH_QT4_WRAP_CPP(contentbrowser_HEADERS_MOC ${QTMOCFILES})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
QT4_WRAP_UI(contentbrowser_FORMS_HEADERS ${FORMS})
NONPCH_QT4_ADD_RESOURCES(contentbrowser_RESOURCES_RCC ${RESOURCES})


LIST(APPEND ALLSRCS ${CODE} ${PCH_files} ${contentbrowser_HEADERS_MOC} ${contentbrowser_FORMS_HEADERS} ${contentbrowser_RESOURCES_RCC} code/contentbrowser.rc)

#STRING(REPLACE "/MTd" "/MDd" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
#STRING(REPLACE "/MT" "/MD" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
REMOVE_DEFINITIONS(-DQT_DEBUG)
STRING(REPLACE "/MANIFEST:NO" "" CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

ADD_EXECUTABLE(contentbrowser ${ALLSRCS})


N_SET_COMPILER_DEFAULTS()
N_SET_QT_PROPERTIES(contentbrowser "N3SDK/QtToolkit")
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(contentbrowser stdneb.h stdneb.cc ../)
ENDIF()
N_SET_EXE_SUBSYSTEM(contentbrowser Windows)
		

ADD_DEPENDENCIES(contentbrowser
	${NEBULA_DEFAULT_LIBS}
	libqimg
	posteffectcontroller
	splash
	nody
	shady
	resourcebrowser
	toolkitutil
	remoteinterface
	remoteinterface_NIDLS
	qttoolkitutil
	editorfeatures
)

TARGET_LINK_LIBRARIES(contentbrowser 
	
	qtfeature
	posteffectcontroller
	libqimg
	splash
	nody
	shady
	resourcebrowser
	miniexporter
	remoteinterface
	remoteinterface_NIDLS
	qttoolkitutil
	editorfeatures
	toolkitutil
	${NEBULA_DEFAULT_LIBS}
	${NEBULA_TOOLKIT_LIBS}
	${N_FBX_LIBS}
	${QT_WIN_LIBS}
	${N_LIBS}
	${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY}	
)
