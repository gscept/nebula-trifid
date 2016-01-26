#-------------------------------------------------------------------------------
# Surface Generator
#-------------------------------------------------------------------------------

PROJECT(surfacegenerator)

		
FILE(GLOB SRC "code/*.cc") 
FILE(GLOB HEAD "code/*.h")
SOURCE_GROUP("surfacegenerator" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB FORMS "ui/*.ui")
FILE(GLOB RESOURCES ${Nebula3_SOURCE_DIR}/code/qttoolkit/*.qrc)
SET(QTMOCFILES "code/surfacegeneratorwindow.h")

SET(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})
PCH_QT4_WRAP_CPP(surfacegenerator_HEADERS_MOC ${QTMOCFILES})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
QT4_WRAP_UI(surfacegenerator_FORMS_HEADERS ${FORMS})
NONPCH_QT4_ADD_RESOURCES(surfacegenerator_RESOURCES_RCC ${RESOURCES})

LIST(APPEND ALLSRCS ${CODE} ${PCH_files} ${surfacegenerator_HEADERS_MOC} ${surfacegenerator_FORMS_HEADERS} ${surfacegenerator_RESOURCES_RCC})

REMOVE_DEFINITIONS(-DQT_DEBUG)

IF(MSVC)
	ADD_DEFINITIONS(/D_HAS_EXCEPTIONS=1)
	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} /EHsc)
ENDIF()

ADD_EXECUTABLE(surfacegenerator ${ALLSRCS})

TARGET_INCLUDE_DIRECTORIES(surfacegenerator PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${Nebula3_SOURCE_DIR}/code/qttoolkit/surfacegenerator/code ${Nebula3_SOURCE_DIR}/code/qttoolkit)

N_SET_COMPILER_DEFAULTS()
N_SET_QT_PROPERTIES(surfacegenerator "N3SDK/QtToolkit")
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(surfacegenerator stdneb.h stdneb.cc ../)
ENDIF()
		
N_SET_EXE_SUBSYSTEM(surfacegenerator Windows)

ADD_DEPENDENCIES(surfacegenerator	
	foundation
	qtfeature
	qttools
	toolkitutil	
)

TARGET_LINK_LIBRARIES(surfacegenerator 
	foundation
	qtfeature
	qttools
	toolkitutil 
	${QT_WIN_LIBS}	
	${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY}	
)
