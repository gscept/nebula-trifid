#-------------------------------------------------------------------------------
# physicsmaterialeditor
#-------------------------------------------------------------------------------

PROJECT(physicsmaterialeditor)

FILE(GLOB SRC "code/*.cc") 
FILE(GLOB HEAD "code/*.h")
FILE(GLOB FORMS "ui/*.ui")
SOURCE_GROUP("physicsmaterialeditor" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

SET(QTMOCFILES "code/physicsmaterialeditor.h")
PCH_QT4_WRAP_CPP(physicsmaterialeditor_HEADERS_MOC ${QTMOCFILES})
QT4_WRAP_UI(physicsmaterialeditor_FORMS_HEADERS ${FORMS})

SET(PCH_files	
    ../../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})


LIST(APPEND ALLSRCS ${CODE} ${PCH_files} ${physicsmaterialeditor_HEADERS_MOC} ${physicsmaterialeditor_FORMS_HEADERS})
ADD_LIBRARY(physicsmaterialeditor STATIC ${ALLSRCS})
TARGET_INCLUDE_DIRECTORIES(physicsmaterialeditor PUBLIC ${Nebula3_SOURCE_DIR}/code/qttoolkit/qtaddons/physicsmaterialeditor/code PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
TARGET_LINK_LIBRARIES(physicsmaterialeditor application toolkitutil)
SET_TARGET_PROPERTIES(physicsmaterialeditor PROPERTIES FOLDER "N3SDK/QtAddons")
N_SET_COMPILER_DEFAULTS()		

