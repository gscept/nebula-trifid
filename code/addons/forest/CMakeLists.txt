#-------------------------------------------------------------------------------
# audio2
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code
					${Nebula3_SOURCE_DIR}/code/addons/
					${Nebula3_SOURCE_DIR}/code/foundation/
					${Nebula3_SOURCE_DIR}/code/extlibs/
					${Nebula3_SOURCE_DIR}/code/render/			
					)
					
      set(SOURCES_files_forest__rt
        rt/foresthandler.cc
        rt/foresthandler.h
        rt/forestrtplugin.cc
        rt/forestrtplugin.h
        rt/forestserver.cc
        rt/forestserver.h
      )
      source_group("forest\\rt" FILES ${SOURCES_files_forest__rt})

      set(SOURCES_forest__rt
        ${SOURCES_files_forest__rt}
      )

    set(SOURCES_files_forest
      forestrendermodule.cc
      forestrendermodule.h
      tree.cc
      tree.h
      treebillboardgraphicsentity.cc
      treebillboardgraphicsentity.h
      treebillboardrenderer.cc
      treebillboardrenderer.h
      treecluster.cc
      treecluster.h
      treeclustergraphicsentity.cc
      treeclustergraphicsentity.h
      treeclusterpool.cc
      treeclusterpool.h
      treeinstance.cc
      treeinstance.h
      treelod.cc
      treelod.h
    )
    source_group("forest" FILES ${SOURCES_files_forest})

    set(SOURCES_forest
      ${SOURCES_forest__rt}
      ${SOURCES_files_forest}
    )

	set(SOURCES_files
		../stdneb.h
		../stdneb.cc
	)

	SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
	set(P_NIDL_FILES forestprotocol.nidl)
	
	N_WRAP_NIDL_FILES(forest_NIDLS ${P_NIDL_FILES})
	
	set(SOURCES_SRCS
		${SOURCES_forest}
		${SOURCES_files}		
	)


add_library( forest STATIC ${SOURCES_SRCS} )
add_dependencies(forest forest_NIDLS)
SET_TARGET_PROPERTIES(forest PROPERTIES FOLDER "N3SDK/addons")

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(forest stdneb.h stdneb.cc ../)
ENDIF()
