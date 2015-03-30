#-------------------------------------------------------------------------------
# editor managers
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(				${Nebula3_SOURCE_DIR}/code/application
									${Nebula3_SOURCE_DIR}/code/addons									
									${Nebula3_SOURCE_DIR}/code/application/basegamefeature
									${Nebula3_SOURCE_DIR}/code/application/corefeature
									${Nebula3_SOURCE_DIR}/code/application/graphicsfeature
									${Nebula3_SOURCE_DIR}/code/application/physicsfeature
									${Nebula3_SOURCE_DIR}/code/extlibs																										
									${Nebula3_SOURCE_DIR}/code/foundation
									${Nebula3_SOURCE_DIR}/code/render
									${Nebula3_SOURCE_DIR}/code/toolkit
									${Nebula3_SOURCE_DIR}/code/toolkit/toolkitutil
									${Nebula3_SOURCE_DIR}/code/
					)
					
    set(SOURCES_files
      editorbasegamefeature.cc
	  editorbasegamefeature.h
	  editorblueprintmanager.h
	  editorblueprintmanager.cc
    )
    source_group("managers" FILES ${SOURCES_files})

    set(SOURCES
      ${SOURCES_files}
    )

	set(SOURCES_files		
	    ../stdneb.cc
	)
	
	SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
	set(SOURCES_SRCS
		${SOURCES}
		${SOURCES_files}
	)

add_library( editorfeatures STATIC ${SOURCES_SRCS} )
SET_TARGET_PROPERTIES(editorfeatures PROPERTIES FOLDER "N3SDK/Toolkit")
N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(editorfeatures stdneb.h stdneb.cc ../)
ENDIF()