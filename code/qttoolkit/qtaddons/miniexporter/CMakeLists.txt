#-------------------------------------------------------------------------------
# miniexporter
#-------------------------------------------------------------------------------

PROJECT(miniexporter)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
					${Nebula3_SOURCE_DIR}/code
					${Nebula3_SOURCE_DIR}/code/foundation
					${Nebula3_SOURCE_DIR}/code/application
					${Nebula3_SOURCE_DIR}/code/extlibs					
					${Nebula3_SOURCE_DIR}/code/render
					${Nebula3_SOURCE_DIR}/code/toolkit
					${Nebula3_SOURCE_DIR}/code/toolkit/toolkitutil
					${Nebula3_SOURCE_DIR}/code/qttoolkit/qtaddons/miniexporter/code)


FILE(GLOB SRC "code/*.cc") 
FILE(GLOB HEAD "code/*.h")
FILE(GLOB FORMS "ui/*.ui")
SOURCE_GROUP("miniexporter" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

N_QT_WRAP_UI(miniexporter_FORMS_HEADERS ${FORMS})

SET(PCH_files	
    ../../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})


LIST(APPEND ALLSRCS ${CODE} ${PCH_files} ${miniexporter_FORMS_HEADERS})
ADD_LIBRARY(miniexporter STATIC ${ALLSRCS})
SET_TARGET_PROPERTIES(miniexporter PROPERTIES FOLDER "N3SDK/QtAddons")
TARGET_LINK_LIBRARIES(miniexporter qtsupport)
N_SET_COMPILER_DEFAULTS()		

