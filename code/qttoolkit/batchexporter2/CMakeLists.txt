#-------------------------------------------------------------------------------
# batchexporter
#-------------------------------------------------------------------------------

PROJECT(bachexporter2)

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
FILE(GLOB RESOURCES ${QTTOOLS}/ui/*.qrc)

PCH_QT4_WRAP_CPP(batchexporter_HEADERS_MOC ${HEAD})
LIST(APPEND SRCS ${SRC} ${HEAD})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
QT4_WRAP_UI(batchexporter_FORMS_HEADERS ${FORMS})
NONPCH_QT4_ADD_RESOURCES(batchexporter_RESOURCES_RCC ${RESOURCES})

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

ADD_EXECUTABLE(batchexporter2 ${ALLSRCS})

N_SET_COMPILER_DEFAULTS()
N_SET_QT_PROPERTIES(batchexporter2 "N3SDK/QtToolkit")
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(batchexporter2 stdneb.h stdneb.cc ../)
ENDIF()
N_SET_EXE_SUBSYSTEM(batchexporter2 Windows)

ADD_DEPENDENCIES(batchexporter2
	foundation
	toolkitutil	
	qttools
)

TARGET_LINK_LIBRARIES(batchexporter2
    toolkitutil
	foundation		
	qttools
	${QT_WIN_LIBS}	
	${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY}
)

FILE(GLOB SOURCE_GROUP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*/tools/*)
SOURCE_GROUP(batchexporter\\tools FILES ${SOURCE_GROUP_FILES})
