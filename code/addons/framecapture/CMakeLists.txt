
#-------------------------------------------------------------------------------
# framecapture
#-------------------------------------------------------------------------------
							
set(SOURCES_files_framecapture__rt
	rt/framecapturehandler.cc
	rt/framecapturehandler.h
	rt/framecapturepagehandler.cc
	rt/framecapturepagehandler.h
	rt/framecapturertplugin.cc
	rt/framecapturertplugin.h
	rt/framecaptureserver.cc
	rt/framecaptureserver.h
)
source_group("framecapture\\rt" FILES ${SOURCES_files_framecapture__rt})

set(SOURCES_framecapture__rt
	${SOURCES_files_framecapture__rt}
)

set(SOURCES_files_framecapture
	framecapturerendermodule.cc
	framecapturerendermodule.h
)
source_group("framecapture" FILES ${SOURCES_files_framecapture})

set(SOURCES_framecapture
	${SOURCES_framecapture__rt}
	${SOURCES_files_framecapture}
)

set(SOURCES_files
	../stdneb.h
	../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
set(P_NIDL_FILES framecaptureprotocol.nidl)
	
	
	
set(SOURCES_SRCS
	${SOURCES_framecapture}
	${SOURCES_files}	
)

N_WRAP_ADD_PROJ_NIDL_FILES(framecapture SOURCES_SRCS ${P_NIDL_FILES})

add_library( framecapture STATIC ${SOURCES_SRCS} )
add_dependencies(framecapture render)
TARGET_LINK_LIBRARIES(framecapture render)
SET_TARGET_PROPERTIES(framecapture PROPERTIES FOLDER "N3SDK/addons")

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(framecapture stdneb.h stdneb.cc ../)
ENDIF()
