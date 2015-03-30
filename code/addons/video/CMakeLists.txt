#-------------------------------------------------------------------------------
# video
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code
					${Nebula3_SOURCE_DIR}/code/addons/
					${Nebula3_SOURCE_DIR}/code/foundation/
					${Nebula3_SOURCE_DIR}/code/extlibs/
					${Nebula3_SOURCE_DIR}/code/render/						
					)


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

    set(SOURCES_WIN32
		win32/win32videoserver.h
		win32/win32videoserver.cc
	)
	source_group("video\\win32" FILES ${SOURCES_WIN32})
	
    set(SOURCES_video
      ${SOURCES_video__base}
      ${SOURCES_video__rt}
      ${SOURCES_files_video}
      ${SOURCES_WIN32}
    )
    
		

	set(SOURCES_files
		../stdneb.h
		../stdneb.cc
	)

	set(P_NIDL_FILES 		
		videoprotocol.nidl	
	)
	
	N_WRAP_NIDL_FILES(video_NIDLS ${P_NIDL_FILES})
	
	set(SOURCES_
		${SOURCES_video}
		${SOURCES_files}		
	)


	add_library( video STATIC ${SOURCES_} )
	add_dependencies(video video_NIDLS)
  SET_TARGET_PROPERTIES(video PROPERTIES FOLDER "N3SDK/addons")

  

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(video stdneb.h stdneb.cc ../)
ENDIF()