#-------------------------------------------------------------------------------
# nebula2 addon
#-------------------------------------------------------------------------------



INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}
					${Nebula3_SOURCE_DIR}/addons/
					${Nebula3_SOURCE_DIR}/foundation/
					${Nebula3_SOURCE_DIR}/extlibs/					
					)
					
    set(SOURCES_files_nebula2
      bbox.cc
      bxmlreader.cc
      frustum.cc
      matrix44.cc
      string.cc
      textwriter.cc
      vector.cc
      xmlreader.cc
      xmlwriter.cc
    )
    source_group("nebula2" FILES ${SOURCES_files_nebula2})

    set(SOURCES_nebula2
      ${SOURCES_files_nebula2}
    )


set(SOURCES_files
	stdneb.h
    ../stdneb.cc
  )
  set(SOURCES_SRCS
    ${SOURCES_nebula2}
    ${SOURCES_files}
  )


  add_library( nebula2_addons STATIC ${SOURCES_SRCS} )


N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(nebula2_addons stdneb.h stdneb.cc ../)
ENDIF()