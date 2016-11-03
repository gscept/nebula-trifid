#-------------------------------------------------------------------------------
# video
#-------------------------------------------------------------------------------

set(SOURCES_files_video__base
	base/videoserverbase.cc
	base/videoserverbase.h
)
source_group("video\\base" FILES ${SOURCES_files_video__base})

set(SOURCES_video__base
	${SOURCES_files_video__base}
)

set(SOURCES_files_video__rt
	rt/videohandler.cc
	rt/videohandler.h
	rt/videortplugin.cc
	rt/videortplugin.h
	rt/videoserver.cc
	rt/videoserver.h
)
source_group("video\\rt" FILES ${SOURCES_files_video__rt})

set(SOURCES_video__rt
	${SOURCES_files_video__rt}
)

set(SOURCES_files_video
    videorendermodule.cc
    videorendermodule.h
)
source_group("video" FILES ${SOURCES_files_video})

set(SOURCES_THEORA
	theora/theoravideoserver.h
	theora/theoravideoserver.cc
	theora/theoravideoplayer.h
	theora/theoravideoplayer.cc
)
source_group("video\\theora" FILES ${SOURCES_THEORA})
	
set(SOURCES_video
    ${SOURCES_video__base}
    ${SOURCES_video__rt}
    ${SOURCES_files_video}
    ${SOURCES_THEORA}
)

set(SOURCES_files
	../stdneb.h
	../stdneb.cc
)

set(P_NIDL_FILES 		
	videoprotocol.nidl	
)
	
set(SOURCES_
	${SOURCES_video}
	${SOURCES_files}		
)
N_WRAP_ADD_PROJ_NIDL_FILES(video SOURCES_ ${P_NIDL_FILES})

add_library( video STATIC ${SOURCES_} )

SET_TARGET_PROPERTIES(video PROPERTIES FOLDER "N3SDK/addons")
TARGET_LINK_LIBRARIES(video PUBLIC theora ogg render)
  

N_SET_COMPILER_DEFAULTS(video)
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(video stdneb.h stdneb.cc ../)
ENDIF()