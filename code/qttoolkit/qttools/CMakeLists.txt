#-------------------------------------------------------------------------------
# batchexporter
#-------------------------------------------------------------------------------

PROJECT(qttools)

SET(QT_USE_QTNETWORK TRUE)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}		
					${Nebula3_SOURCE_DIR}/code
					${Nebula3_SOURCE_DIR}/code/foundation
					${Nebula3_SOURCE_DIR}/code/foundation/net
					${Nebula3_SOURCE_DIR}/code/render
					${Nebula3_SOURCE_DIR}/code/toolkit			
					${Nebula3_SOURCE_DIR}/code/qttoolkit/qttools/code)


FILE(GLOB_RECURSE SRC "code/*.cc") 
FILE(GLOB_RECURSE HEAD "code/*.h")

SET(QTMOCS "code/tools/progressnotifier.h" "code/style/stylewidgets.h" )

PCH_QT_WRAP_CPP(qttools_HEADERS_MOC ${QTMOCS})

LIST(APPEND SRCS ${SRC} ${HEAD} ${qttools_FORMS_HEADERS})
SOURCE_GROUP("qttools" FILES ${SRCS})

set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCS} ${qttools_HEADERS_MOC} ${PCH_files})

#STRING(REPLACE "/MTd" "/MDd" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
#STRING(REPLACE "/MT" "/MD" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
#STRING(REPLACE "/MANIFEST:NO" "" CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

ADD_LIBRARY(qttools STATIC ${ALLSRCS})
SET_TARGET_PROPERTIES(qttools PROPERTIES FOLDER "N3SDK/QtToolkit")
TARGET_INCLUDE_DIRECTORIES(qttools PUBLIC ${Nebula3_SOURCE_DIR}/code/qttoolkit/qttools/code PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

TARGET_LINK_LIBRARIES(qttools application toolkitutil qtsupport)

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(qttools stdneb.h stdneb.cc ../)
ENDIF()


FILE(GLOB SOURCE_GROUP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*/tools/*)
SOURCE_GROUP(tools FILES ${SOURCE_GROUP_FILES})
FILE(GLOB SOURCE_GROUP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*/output/*)
SOURCE_GROUP(output FILES ${SOURCE_GROUP_FILES})
FILE(GLOB SOURCE_GROUP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*/style/*)
SOURCE_GROUP(style FILES ${SOURCE_GROUP_FILES})
