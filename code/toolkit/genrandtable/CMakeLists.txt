#-------------------------------------------------------------------------------
# Tools
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(				${Nebula3_SOURCE_DIR}/code/application
									${Nebula3_SOURCE_DIR}/code/addons									
									${Nebula3_SOURCE_DIR}/code/application/basegamefeature
									${Nebula3_SOURCE_DIR}/code/application/corefeature
									${Nebula3_SOURCE_DIR}/code/application/graphicsfeature
									${Nebula3_SOURCE_DIR}/code/application/physicsfeature
									${Nebula3_SOURCE_DIR}/code/extlibs
									${Nebula3_SOURCE_DIR}/code/extlibs/ode
									${Nebula3_SOURCE_DIR}/code/foundation
									${Nebula3_SOURCE_DIR}/code/render
									${Nebula3_SOURCE_DIR}/code/sui
									${Nebula3_SOURCE_DIR}/code/tools		
									${Nebula3_SOURCE_DIR}/code
					)		
					
    set(SOURCES_files_leveleditortestapplication
      genrandtable.cc genrandtableapplication.cc genrandtableapplication.h
    )
    source_group("genrandtable" FILES ${SOURCES_files_leveleditortestapplication})

    set(SOURCES_leveleditortestapplication
      ${SOURCES_files_leveleditortestapplication}
    )

  set(SOURCES_files	
    ../stdneb.cc
  )

  set(SOURCES_SRCS
    ${SOURCES_leveleditortestapplication}
    ${SOURCES_files}
  )

	ADD_EXECUTABLE(genrandtable ${SOURCES_SRCS})
	SET_TARGET_PROPERTIES(genrandtable PROPERTIES FOLDER "N3SDK/Toolkit")
	
	N_SET_COMPILER_DEFAULTS()
	N_SET_EXE_LINKER_DEFAULTS(genrandtable)
	N_SET_EXE_SUBSYSTEM(genrandtable Windows)
	IF(N_USE_PRECOMPILED_HEADERS)
		N_SET_PCH(genrandtable stdneb.h stdneb.cc ../)
	ENDIF()
	ADD_DEPENDENCIES(genrandtable
		foundation
		zlib
		tinyxml
		)
	TARGET_LINK_LIBRARIES(genrandtable 
		foundation
		zlib
		tinyxml
		${N_LIBS}
	)