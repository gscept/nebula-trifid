#-------------------------------------------------------------------------------
# globalprobecontroller
#-------------------------------------------------------------------------------

PROJECT(globalprobecontroller)

FILE(GLOB SRC "code/*.cc") 
FILE(GLOB HEAD "code/*.h")
FILE(GLOB FORMS "ui/*.ui")
SOURCE_GROUP("globalprobecontroller" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

SET(QTMOCFILES "code/environmentprobewindow.h")
			   
PCH_QT_WRAP_CPP(globalprobecontroller_HEADERS_MOC ${QTMOCFILES})
N_QT_WRAP_UI(globalprobecontroller_FORMS_HEADERS ${FORMS})

SET(PCH_files	
    ../../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${CODE} ${PCH_files} ${globalprobecontroller_HEADERS_MOC} ${globalprobecontroller_FORMS_HEADERS})
ADD_LIBRARY(globalprobecontroller STATIC ${ALLSRCS})
TARGET_INCLUDE_DIRECTORIES(globalprobecontroller PUBLIC ${Nebula3_SOURCE_DIR}/code/qttoolkit/qtaddons/globalprobecontroller/code PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
TARGET_LINK_LIBRARIES(globalprobecontroller application toolkitutil resourcebrowser qtsupport)
SET_TARGET_PROPERTIES(globalprobecontroller PROPERTIES FOLDER "N3SDK/QtAddons")
N_SET_COMPILER_DEFAULTS()		

