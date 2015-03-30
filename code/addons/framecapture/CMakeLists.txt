
#-------------------------------------------------------------------------------
# framecapture
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(../
					${Nebula3_SOURCE_DIR}/code/addons/
					${Nebula3_SOURCE_DIR}/code/foundation/
					${Nebula3_SOURCE_DIR}/code/render/
					${Nebula3_SOURCE_DIR}/code/extlibs/
					${Nebula3_SOURCE_DIR}/code
					)
					
					
      set(SOURCES_files_framecapture__rt
        rt/framecapturehandler.cc
        rt/framecapturehandler.h
        rt/framecapturepagehandler.cc
        rt/framecapturepagehandler.h
        rt/framecapturertplugin.cc
        rt/framecapturertplugin.h
        rt/framecaptureserver.cc
        rt/framecaptureserver.h
      )
      source_group("framecapture\\rt" FILES ${SOURCES_files_framecapture__rt})

      set(SOURCES_framecapture__rt
        ${SOURCES_files_framecapture__rt}
      )

    set(SOURCES_files_framecapture
      framecapturerendermodule.cc
      framecapturerendermodule.h
    )
    source_group("framecapture" FILES ${SOURCES_files_framecapture})

    set(SOURCES_framecapture
      ${SOURCES_framecapture__rt}
      ${SOURCES_files_framecapture}
    )

	set(SOURCES_files
		../stdneb.h
		../stdneb.cc
	)
	SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
	set(P_NIDL_FILES framecaptureprotocol.nidl)
	
	N_WRAP_NIDL_FILES(framecapture_NIDLS ${P_NIDL_FILES})
	
	set(SOURCES_SRCS
		${SOURCES_framecapture}
		${SOURCES_files}	
	)


add_library( framecapture STATIC ${SOURCES_SRCS} )
add_dependencies(framecapture framecapture_NIDLS)
SET_TARGET_PROPERTIES(framecapture PROPERTIES FOLDER "N3SDK/addons")

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(framecapture stdneb.h stdneb.cc ../)
ENDIF()
