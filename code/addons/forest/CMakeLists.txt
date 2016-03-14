#-------------------------------------------------------------------------------
# forest
#-------------------------------------------------------------------------------
			
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
	

	
set(SOURCES_SRCS
	${SOURCES_forest}
	${SOURCES_files}		
)
N_WRAP_ADD_NIDL_FILES(forest SOURCES_SRCS ${P_NIDL_FILES})

add_library( forest STATIC ${SOURCES_SRCS})
SET_TARGET_PROPERTIES(forest PROPERTIES FOLDER "N3SDK/addons")
TARGET_LINK_LIBRARIES(forest application)

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(forest stdneb.h stdneb.cc ../)
ENDIF()
