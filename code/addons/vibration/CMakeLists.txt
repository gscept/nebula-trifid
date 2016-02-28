#-------------------------------------------------------------------------------
# vibration
#-------------------------------------------------------------------------------

set(SOURCES_files_posteffect
	vibrationhandler.cc
	vibrationhandler.h
	vibrationinterface.cc
	vibrationinterface.h
	vibrationplayer.cc
	vibrationplayer.h	
)
source_group("vibration" FILES ${SOURCES_files_posteffect})

set(SOURCES_posteffect
	${SOURCES_posteffect__rt}
	${SOURCES_files_posteffect}
)

set(SOURCES_files
	../stdneb.h
	../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
set(P_NIDL_FILES vibrationprotocol.nidl)
	
	
set(SOURCES_
	${SOURCES_posteffect}
	${SOURCES_files}		
)
N_WRAP_ADD_NIDL_FILES(vibration SOURCES_ ${P_NIDL_FILES})

add_library( vibration STATIC ${SOURCES_} )
add_dependencies(vibration application)
TARGET_LINK_LIBRARIES(vibration PUBLIC render)

SET_TARGET_PROPERTIES(vibration PROPERTIES FOLDER "N3SDK/addons")
  
N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(vibration stdneb.h stdneb.cc ../)
ENDIF()
 
