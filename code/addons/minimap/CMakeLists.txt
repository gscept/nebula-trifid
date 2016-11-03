#-------------------------------------------------------------------------------
# minimap
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(..
					${Nebula3_SOURCE_DIR}/code/addons/
					${Nebula3_SOURCE_DIR}/code/foundation/
					${Nebula3_SOURCE_DIR}/code/render/
					${Nebula3_SOURCE_DIR}/code/application/
					${Nebula3_SOURCE_DIR}/code
					)
					
    set(SOURCES_files_minimap
		minimapproperty.cc
		minimapproperty.h
		minimapmanager.cc
		minimapmanager.h
		minimapplugin.cc
		minimapplugin.h		
    )
    source_group("minimap" FILES ${SOURCES_files_minimap})
	SET(ROCKET_SRC 
		rocket/elementminimap.cc
		rocket/elementminimap.h	
	)
	source_group("minimap\\rocket" FILES ${ROCKET_SRC})

    set(SOURCES_minimap
      ${SOURCES_files_minimap}
	  ${ROCKET_SRC}
    )

  set(SOURCES_files
	../stdneb.h
    ../stdneb.cc
  )
	
N_WRAP_ADD_NIDL_FILES(SOURCES_minimap minimap.nidl)
	
	SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
	set(SOURCES_SRCS
	    ${SOURCES_minimap}
	    ${SOURCES_files}
	)

add_library( minimap STATIC ${SOURCES_SRCS} )
TARGET_LINK_LIBRARIES(minimap render ui)
SET_TARGET_PROPERTIES(minimap PROPERTIES FOLDER "N3SDK/addons")
N_SET_COMPILER_DEFAULTS(minimap)
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(minimap stdneb.h stdneb.cc ../)
ENDIF()
