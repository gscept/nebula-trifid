#-------------------------------------------------------------------------------
# remoteinterface
#-------------------------------------------------------------------------------

PROJECT(remoteinterface)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
					${Nebula3_SOURCE_DIR}/code
					${Nebula3_SOURCE_DIR}/code/foundation
					${Nebula3_SOURCE_DIR}/code/foundation/net					
					${Nebula3_SOURCE_DIR}/code/render
					${Nebula3_SOURCE_DIR}/code/application
					${Nebula3_SOURCE_DIR}/code/extlibs
					${Nebula3_SOURCE_DIR}/code/toolkit
					${Nebula3_SOURCE_DIR}/code/toolkit/toolkitutil
					${Nebula3_SOURCE_DIR}/code/addons
					${Nebula3_SOURCE_DIR}/code/qttoolkit/qtaddons/remoteinterface)


FILE(GLOB SRC "*.cc") 
FILE(GLOB HEAD "*.h")

SOURCE_GROUP("remoteinterface" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})


SET(QTMOCFILES qtremoteclient.h)

SET(PCH_files	
    ../../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})
PCH_QT4_WRAP_CPP(remoteinterface_HEADERS_MOC ${QTMOCFILES})

SET(qttoolkitutil_NIDLS
	qtremoteprotocol.nidl
)
N_WRAP_NIDL_FILES(remoteinterface_NIDLS ${qttoolkitutil_NIDLS})

LIST(APPEND ALLSRCS ${CODE} ${PCH_files} ${remoteinterface_HEADERS_MOC})
ADD_LIBRARY(remoteinterface STATIC ${ALLSRCS})
ADD_DEPENDENCIES(remoteinterface remoteinterface_NIDLS)
SET_TARGET_PROPERTIES(remoteinterface PROPERTIES FOLDER "N3SDK/QtAddons")
N_SET_COMPILER_DEFAULTS()		

