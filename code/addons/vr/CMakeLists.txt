#-------------------------------------------------------------------------------
# vr
#-------------------------------------------------------------------------------

OPTION(N_VR_SUPPORT "Enable OpenVR support." OFF)

IF(N_VR_SUPPORT)

set(SOURCES_vr 
	vrmanager.cc
	vrmanager.h
	vrview.h
	vrview.cc
	vrcameraproperty.cc
	vrcameraproperty.h
	vivemote.cc
	vivemote.h
	viveinteractproperty.cc
	viveinteractproperty.h
)
source_group("vr" FILES ${SOURCES_vr})

set(SOURCES_files
	../stdneb.h
	../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})

set(P_NIDL_FILES 		
	vrprotocol.nidl	
)

SET(OPENVR_ROOT_DIR ${Nebula3_SOURCE_DIR}/code/extlibs/openvr)
FIND_PACKAGE(OpenVR REQUIRED)
	
N_WRAP_ADD_PROJ_NIDL_FILES(vr SOURCES_vr ${P_NIDL_FILES})

add_library( vr STATIC ${SOURCES_vr} ${SOURCES_files} )

SET_TARGET_PROPERTIES(vr PROPERTIES FOLDER "N3SDK/addons")
TARGET_INCLUDE_DIRECTORIES(vr PRIVATE ${OPENVR_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(vr PUBLIC application graphicsfeature ${OPENVR_LIBRARY})
  

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(vr stdneb.h stdneb.cc ../)
ENDIF()

ENDIF()