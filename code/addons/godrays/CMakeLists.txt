#-------------------------------------------------------------------------------
# godrays
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/addons/
					${Nebula3_SOURCE_DIR}/code/foundation/
					${Nebula3_SOURCE_DIR}/code/render/
					${Nebula3_SOURCE_DIR}/code
					)
					
    set(SOURCES_files_godrays
		godrayrendermodule.h
		godrayrendermodule.cc
    )
    source_group("godrays" FILES ${SOURCES_files_godrays})

    set(SOURCES_godrays
		${SOURCES_files_godrays}
    )
	
	set(SOURCES_files_godrays__rt
		rt/godrayhandler.cc
		rt/godrayhandler.h
		rt/godrayrtplugin.cc
		rt/godrayrtplugin.h
		rt/godrayserver.cc
		rt/godrayserver.h
	)
	source_group("godrays\\rt" FILES ${SOURCES_files_godrays__rt})

	set(SOURCES_godrays__rt
		${SOURCES_files_godrays__rt}
	)

	set(SOURCES_files
	../stdneb.h
	../stdneb.cc
	)
	
	SET(P_NIDL_FILES godrayprotocol.nidl)
	N_WRAP_NIDL_FILES(godrays_NIDLS ${P_NIDL_FILES})
	
	SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
	set(SOURCES_SRCS
		${SOURCES_godrays}
		${SOURCES_files_godrays__rt}
		${SOURCES_files}	
	)

add_library( godrays STATIC ${SOURCES_SRCS} )
add_dependencies(godrays godrays_NIDLS)
SET_TARGET_PROPERTIES(godrays PROPERTIES FOLDER "N3SDK/addons/rendereffects")
N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(godrays stdneb.h stdneb.cc ../)
ENDIF()
