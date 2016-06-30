#-------------------------------------------------------------------------------
# remoteinterface
#-------------------------------------------------------------------------------

PROJECT(remoteinterface)

FILE(GLOB SRC "*.cc") 
FILE(GLOB HEAD "*.h")

SOURCE_GROUP("remoteinterface" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})


SET(QTMOCFILES qtremoteclient.h)

SET(PCH_files	
    ../../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})
PCH_QT_WRAP_CPP(remoteinterface_HEADERS_MOC ${QTMOCFILES})

SET(qttoolkitutil_NIDLS
	qtremoteprotocol.nidl
)
N_WRAP_ADD_PROJ_NIDL_FILES(remoteinterface CODE ${qttoolkitutil_NIDLS})

LIST(APPEND ALLSRCS ${CODE} ${PCH_files} ${remoteinterface_HEADERS_MOC})
ADD_LIBRARY(remoteinterface STATIC ${ALLSRCS})
TARGET_LINK_LIBRARIES(remoteinterface PUBLIC application toolkitutil qtsupport )
TARGET_INCLUDE_DIRECTORIES(remoteinterface PUBLIC ${Nebula3_SOURCE_DIR}/code/qttoolkit/qtaddons/ PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
SET_TARGET_PROPERTIES(remoteinterface PROPERTIES FOLDER "N3SDK/QtAddons")
N_SET_COMPILER_DEFAULTS()		

