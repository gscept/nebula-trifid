#-------------------------------------------------------------------------------
# grid
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(..
					${Nebula3_SOURCE_DIR}/code/addons/
					${Nebula3_SOURCE_DIR}/code/foundation/
					${Nebula3_SOURCE_DIR}/code/render/
					${Nebula3_SOURCE_DIR}/code
					)
					
    set(SOURCES_files_grid
		gridaddon.h
		gridaddon.cc
		gridrtplugin.h
		gridrtplugin.cc
    )
    source_group("grid" FILES ${SOURCES_files_grid})

    set(SOURCES_grid
      ${SOURCES_files_grid}
    )

  set(SOURCES_files
	../stdneb.h
    ../stdneb.cc
  )
	
	SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
	set(SOURCES_SRCS
	    ${SOURCES_grid}
	    ${SOURCES_files}
	)

add_library( grid STATIC ${SOURCES_SRCS} )
SET_TARGET_PROPERTIES(grid PROPERTIES FOLDER "N3SDK/addons")
N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(grid stdneb.h stdneb.cc ../)
ENDIF()
