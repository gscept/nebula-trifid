#-------------------------------------------------------------------------------
# vegetation
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(..
					${Nebula3_SOURCE_DIR}/addons/
					${Nebula3_SOURCE_DIR}/foundation/
					${Nebula3_SOURCE_DIR}/render/	
					${Nebula3_SOURCE_DIR}
					)
					
    set(SOURCES_files_attr
      emptyresourceloader.cc
emptyresourceloader.h
grassentityeventhandler.cc
grassentityeventhandler.h
grassgroundmap.cc
grassgroundmap.h
grassheightmap.cc
grassheightmap.h
grasslayer.cc
grasslayer.h
grasslayerinfo.h
grasslibrary.cc
grasslibrary.h
grasspatch.cc
grasspatch.h
grasspatcharray.cc
grasspatcharray.h
grasspatchcoords.cc
grasspatchcoords.h
grassplantmesh.cc
grassplantmesh.h
grassrenderer.cc
grassrenderer.h
    )
    source_group("vegetation" FILES ${SOURCES_files_attr})

    set(SOURCES_attr
      ${SOURCES_files_attr}
    )

  set(SOURCES_files
	stdneb.h
    ../stdneb.cc
  )

  set(SOURCES_SRCS
    ${SOURCES_attr}
    ${SOURCES_files}
  )

add_library( vegetation STATIC ${SOURCES_SRCS} )
SET_TARGET_PROPERTIES(vegetation PROPERTIES FOLDER "N3SDK/addons")
N_SET_COMPILER_DEFAULTS()

IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(vegetation stdneb.h stdneb.cc ../)
ENDIF()