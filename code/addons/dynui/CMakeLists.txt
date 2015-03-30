#-------------------------------------------------------------------------------
# dynui addon
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(./
					${Nebula3_SOURCE_DIR}/code/addons/
					${Nebula3_SOURCE_DIR}/code/foundation/
					${Nebula3_SOURCE_DIR}/code/render/
					${Nebula3_SOURCE_DIR}/code
					${Nebula3_SOURCE_DIR}/code/extlibs
					${Nebula3_SOURCE_DIR}/code/application
					)
					
    set(SOURCES_files_dynui
		imguiaddon.h
		imguiaddon.cc
		imguiinputhandler.h
		imguiinputhandler.cc
		imguirenderer.h
		imguirenderer.cc
		imguirtplugin.h
		imguirtplugin.cc
    )
    source_group("dynui" FILES ${SOURCES_files_dynui})

	set(SOURCES_files_dynui__console
		console/imguiconsole.h
		console/imguiconsole.cc
		console/imguiconsolehandler.h
		console/imguiconsolehandler.cc
	)
	source_group("dynui\\console" FILES ${SOURCES_files_dynui__console})

	set(SOURCES_dynui
      ${SOURCES_files_dynui}
	  ${SOURCES_files_dynui__console}
    )
	
  set(SOURCES_files
	../stdneb.h
    ../stdneb.cc
  )
	
	SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
	set(SOURCES_SRCS
	    ${SOURCES_dynui}
	    ${SOURCES_files}
	)

add_library( dynui STATIC ${SOURCES_SRCS} )
SET_TARGET_PROPERTIES(dynui PROPERTIES FOLDER "N3SDK/addons")
N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(dynui stdneb.h stdneb.cc ../)
ENDIF()
