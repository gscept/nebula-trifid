#-------------------------------------------------------------------------------
# batchexporter
#-------------------------------------------------------------------------------

PROJECT(bachexporter)

SET(QT_USE_QTNETWORK TRUE)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
                    ${Nebula3_SOURCE_DIR}/code
                    ${Nebula3_SOURCE_DIR}/code/foundation
                    ${Nebula3_SOURCE_DIR}/code/foundation/net
                    ${Nebula3_SOURCE_DIR}/code/render
                    ${Nebula3_SOURCE_DIR}/code/toolkit
                    ${Nebula3_SOURCE_DIR}/code/toolkit/toolkitutil
                    ${Nebula3_SOURCE_DIR}/code/qttoolkit/
                    ${Nebula3_SOURCE_DIR}/code/qttoolkit/qttools/code
                    ${Nebula3_SOURCE_DIR}/code/qttoolkit/batchexporter/code)

FILE(GLOB_RECURSE SRC "code/*.cc") 
FILE(GLOB_RECURSE HEAD "code/*.h")

FILE(GLOB FORMS "ui/*.ui")
FILE(GLOB RESOURCES ${Nebula3_SOURCE_DIR}/code/qttoolkit/qttools/ui/*.qrc)
FILE(GLOB RESOURCES_2 ${Nebula3_SOURCE_DIR}/code/qttoolkit/*.qrc)

PCH_QT_WRAP_CPP(batchexporter_HEADERS_MOC ${HEAD})
LIST(APPEND SRCS ${SRC} ${HEAD})

N_QT_WRAP_UI(batchexporter_FORMS_HEADERS ${FORMS})
NONPCH_QT_ADD_RESOURCES(batchexporter_RESOURCES_RCC ${RESOURCES} ${RESOURCES_2})

SOURCE_GROUP("batchexporter" FILES ${SRCS})

SET(PCH_files
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})
LIST(APPEND ALLSRCS ${SRCS} ${PCH_files} ${batchexporter_HEADERS_MOC} ${batchexporter_FORMS_HEADERS} ${batchexporter_RESOURCES_RCC} code/batchexporter.rc)

#STRING(REPLACE "/MTd" "/MDd" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
#STRING(REPLACE "/MT" "/MD" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
REMOVE_DEFINITIONS(-DQT_DEBUG)
#STRING(REPLACE "/MANIFEST:NO" "" CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

ADD_EXECUTABLE(batchexporter ${ALLSRCS})

N_SET_COMPILER_DEFAULTS()
N_SET_QT_PROPERTIES(batchexporter "N3SDK/QtToolkit")
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(batchexporter stdneb.h stdneb.cc ../)
ENDIF()
N_SET_EXE_SUBSYSTEM(batchexporter Windows)

ADD_DEPENDENCIES(batchexporter
	foundation
	toolkitutil	
	qttools
)

TARGET_LINK_LIBRARIES(batchexporter 
    toolkitutil
	foundation		
	qttools
	qtsupport
)

FILE(GLOB SOURCE_GROUP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*/tools/*)
SOURCE_GROUP(batchexporter\\tools FILES ${SOURCE_GROUP_FILES})
