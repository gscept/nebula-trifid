#-------------------------------------------------------------------------------
# Surface Generator
#-------------------------------------------------------------------------------

PROJECT(particleconverter)

		
FILE(GLOB SRC "code/*.cc") 
FILE(GLOB HEAD "code/*.h")
SOURCE_GROUP("particleconverter" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB FORMS "ui/*.ui")
FILE(GLOB RESOURCES ${Nebula3_SOURCE_DIR}/code/qttoolkit/*.qrc)
SET(QTMOCFILES "code/particleconverterwindow.h")

SET(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})
PCH_QT4_WRAP_CPP(particleconverter_HEADERS_MOC ${QTMOCFILES})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
QT4_WRAP_UI(particleconverter_FORMS_HEADERS ${FORMS})
NONPCH_QT4_ADD_RESOURCES(particleconverter_RESOURCES_RCC ${RESOURCES})

LIST(APPEND ALLSRCS ${CODE} ${PCH_files} ${particleconverter_HEADERS_MOC} ${particleconverter_FORMS_HEADERS} ${particleconverter_RESOURCES_RCC})

REMOVE_DEFINITIONS(-DQT_DEBUG)

N_SET_COMPILER_DEFAULTS()

ADD_EXECUTABLE(particleconverter ${ALLSRCS})

TARGET_INCLUDE_DIRECTORIES(particleconverter PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${Nebula3_SOURCE_DIR}/code/qttoolkit/particleconverter/code ${Nebula3_SOURCE_DIR}/code/qttoolkit)


N_SET_QT_PROPERTIES(particleconverter "N3SDK/QtToolkit")
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(particleconverter stdneb.h stdneb.cc ../)
ENDIF()
		
N_SET_EXE_SUBSYSTEM(particleconverter Windows)

ADD_DEPENDENCIES(particleconverter	
	foundation
	qtfeature
	qttools
	toolkitutil	
)

TARGET_LINK_LIBRARIES(particleconverter 
	foundation
	qtfeature
	qttools
	toolkitutil 
	${QT_WIN_LIBS}	
	${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY}	
)
