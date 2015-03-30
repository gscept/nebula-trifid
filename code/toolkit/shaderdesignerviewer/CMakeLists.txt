#-------------------------------------------------------------------------------
# shaderdesignerviewer
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(				${Nebula3_SOURCE_DIR}/application
									${Nebula3_SOURCE_DIR}/addons									
									${Nebula3_SOURCE_DIR}/application/basegamefeature
									${Nebula3_SOURCE_DIR}/application/corefeature
									${Nebula3_SOURCE_DIR}/application/graphicsfeature
									${Nebula3_SOURCE_DIR}/application/physicsfeature
									${Nebula3_SOURCE_DIR}/extlibs
									${Nebula3_SOURCE_DIR}/extlibs/ode
									${Nebula3_SOURCE_DIR}/foundation
									${Nebula3_SOURCE_DIR}/render
									${Nebula3_SOURCE_DIR}/sui
									${Nebula3_SOURCE_DIR}/toolkit
									${Nebula3_SOURCE_DIR}/toolkit/shaderdesignerviewer
									${Nebula3_SOURCE_DIR}
					)
link_directories(${N_LIBDIRS})	

	set(SOURCES_files_shaderdesignerviewer
	shaderdesignerviewer.cc
	shaderdesignerviewerapplication.cc
	shaderdesignerviewerapplication.h
	)
	source_group("shaderdesignerviewer" FILES ${SOURCES_files_shaderdesignerviewer})

	set(SOURCES_shaderdesignerviewer
	${SOURCES_files_shaderdesignerviewer}
	)
  
	set(SOURCES_files_shaderdesignerviewer__handlers
		handlers/nodyhandler.cc
		handlers/nodyhandler.h
	)
	source_group("shaderdesignerviewer\\handlers" FILES ${SOURCES_files_shaderdesignerviewer__handlers})
	
	set(SOURCES_shaderdesignerviewer__handlers
		${SOURCES_files_shaderdesignerviewer__handlers}
	)
	  
	set(SOURCES_files
	../stdneb.cc
	)

	set(SOURCES_
	${SOURCES_shaderdesignerviewer}
	${SOURCES_shaderdesignerviewer__handlers}
	${SOURCES_files}
	)


	add_executable( shaderdesignerviewer WIN32 ${SOURCES_} )
	SET_TARGET_PROPERTIES(shaderdesignerviewer PROPERTIES FOLDER "N3SDK/Toolkit")

	N_SET_COMPILER_DEFAULTS()
	N_SET_EXE_LINKER_DEFAULTS(shaderdesignerviewer)
	N_SET_EXE_SUBSYSTEM(shaderdesignerviewer Windows)
	IF(N_USE_PRECOMPILED_HEADERS)
		N_SET_PCH(shaderdesignerviewer stdneb.h stdneb.cc ../)
	ENDIF()
	ADD_DEPENDENCIES(shaderdesignerviewer sui audio2 remote attr vibration tinyxml zlib sqlite db posteffect render_visibilityboxsystemjob render_visibilityquadtreejob
		render_animjobsamplemix render_animjobsample render_particlejob render_charevalskeletonjob
		foundation
		${PHYSICS_DEPENDENCIES}
		render sui
		application)
	TARGET_LINK_LIBRARIES(shaderdesignerviewer 
		sui audio2 remote attr vibration tinyxml zlib sqlite db posteffect render_visibilityboxsystemjob render_visibilityquadtreejob
		render_animjobsamplemix render_animjobsample render_particlejob render_charevalskeletonjob
		foundation
		render
		${PHYSICS_DEPENDENCIES}
		application sui
		${N_LIBS}
	)