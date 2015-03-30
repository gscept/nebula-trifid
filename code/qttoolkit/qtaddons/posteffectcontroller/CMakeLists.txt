#-------------------------------------------------------------------------------
# posteffectcontroller
#-------------------------------------------------------------------------------

PROJECT(posteffectcontroller)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
					${Nebula3_SOURCE_DIR}/code
					${Nebula3_SOURCE_DIR}/code/foundation
					${Nebula3_SOURCE_DIR}/code/foundation/net
					${Nebula3_SOURCE_DIR}/code/application
					${Nebula3_SOURCE_DIR}/code/render
					${Nebula3_SOURCE_DIR}/code/toolkit
					${Nebula3_SOURCE_DIR}/code/toolkit/toolkitutil
					${Nebula3_SOURCE_DIR}/code/addons
					${Nebula3_SOURCE_DIR}/code/qttoolkit/qtaddons/posteffectcontroller/code)


FILE(GLOB SRC "code/*.cc") 
FILE(GLOB HEAD "code/*.h")
FILE(GLOB FORMS "ui/*.ui")
SOURCE_GROUP("posteffectcontroller" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

SET(QTMOCFILES "code/posteffectcontroller.h")
PCH_QT4_WRAP_CPP(posteffectcontroller_HEADERS_MOC ${QTMOCFILES})
QT4_WRAP_UI(posteffectcontroller_FORMS_HEADERS ${FORMS})

SET(PCH_files	
    ../../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})


LIST(APPEND ALLSRCS ${CODE} ${PCH_files} ${posteffectcontroller_HEADERS_MOC} ${posteffectcontroller_FORMS_HEADERS})
ADD_LIBRARY(posteffectcontroller STATIC ${ALLSRCS})
SET_TARGET_PROPERTIES(posteffectcontroller PROPERTIES FOLDER "N3SDK/QtAddons")
N_SET_COMPILER_DEFAULTS()		

