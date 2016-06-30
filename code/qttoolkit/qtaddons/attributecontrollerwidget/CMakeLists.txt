#-------------------------------------------------------------------------------
# attributecontrollerwidget
#-------------------------------------------------------------------------------

PROJECT(attributecontrollerwidget)


FILE(GLOB SRC "code/*.cc") 
FILE(GLOB HEAD "code/*.h")
FILE(GLOB FORMS "ui/*.ui")
SOURCE_GROUP("attributecontrollerwidget" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

SET(QTMOCFILES "code/attributecontrollerwidget.h"
			   "code/decimalnumbercontroller.h"
			   "code/integercontroller.h"
			   "code/stringcontroller.h"
			   "code/resourcestringcontroller.h"
			   "code/stringlistcontroller.h"
			   "code/animclipcontroller.h"
			   "code/float4controller.h"
			   "code/colourcontroller.h"
			   "code/boolcontroller.h"
			   "code/guidcontroller.h"
			   "code/matrix44controller.h"
			   "code/transformcontroller.h"
			   "code/qluatextedit.h"
			   "code/qluasyntaxhighlighter.h"
			   "code/physicsmaterialcontroller.h"
			   "code/scripteditor.h")
			   
PCH_QT_WRAP_CPP(attributecontrollerwidget_HEADERS_MOC ${QTMOCFILES})
N_QT_WRAP_UI(attributecontrollerwidget_FORMS_HEADERS ${FORMS})

SET(PCH_files	
    ../../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${CODE} ${PCH_files} ${attributecontrollerwidget_HEADERS_MOC} ${attributecontrollerwidget_FORMS_HEADERS})
ADD_LIBRARY(attributecontrollerwidget STATIC ${ALLSRCS})
TARGET_INCLUDE_DIRECTORIES(attributecontrollerwidget PUBLIC ${Nebula3_SOURCE_DIR}/code/qttoolkit/qtaddons/attributecontrollerwidget/code PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
TARGET_LINK_LIBRARIES(attributecontrollerwidget application editorfeatures toolkitutil qtsupport)
SET_TARGET_PROPERTIES(attributecontrollerwidget PROPERTIES FOLDER "N3SDK/QtAddons")
N_SET_COMPILER_DEFAULTS()		

