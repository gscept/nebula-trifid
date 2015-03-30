#-------------------------------------------------------------------------------
# silhouette
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(..
					${Nebula3_SOURCE_DIR}/code/addons/
					${Nebula3_SOURCE_DIR}/code/foundation/
					${Nebula3_SOURCE_DIR}/code/render/
					${Nebula3_SOURCE_DIR}/code
					)
					
    set(SOURCES_files_silhouette
		silhouetteaddon.h
		silhouetteaddon.cc
		silhouettertplugin.h
		silhouettertplugin.cc
    )
    source_group("silhouette" FILES ${SOURCES_files_silhouette})

    set(SOURCES_silhouette
      ${SOURCES_files_silhouette}
    )

  set(SOURCES_files
	../stdneb.h
    ../stdneb.cc
  )
	
	SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
	set(SOURCES_SRCS
	    ${SOURCES_silhouette}
	    ${SOURCES_files}
	)

add_library( silhouette STATIC ${SOURCES_SRCS} )
SET_TARGET_PROPERTIES(silhouette PROPERTIES FOLDER "N3SDK/addons")
N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(silhouette stdneb.h stdneb.cc ../)
ENDIF()
