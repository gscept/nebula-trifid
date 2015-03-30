#-------------------------------------------------------------------------------
# sequencer
#-------------------------------------------------------------------------------



INCLUDE_DIRECTORIES(
					${Nebula3_SOURCE_DIR}/addons/
					${Nebula3_SOURCE_DIR}/foundation/
					${Nebula3_SOURCE_DIR}/extlibs/
					)

					
    set(SOURCES_files_sequencer
		animatedparameter.h
		beziercurve.cc
		beziercurve.h
		beziergraph.cc
		beziergraph.h
		cubicequationsolver.cc
		cubicequationsolver.h
		sequencerfeatureunit.cc
		sequencerfeatureunit.h
		sequencermanager.cc
		sequencermanager.h
		track.cc
		track.h
		trackbar.cc
		trackbar.h
		tracksequencer.cc
		tracksequencer.h
		xmlattrparser.cc
		xmlattrparser.h
    )
    source_group("sequencer" FILES ${SOURCES_files_sequencer})

    set(SOURCES_sequencer
      ${SOURCES_files_sequencer}
    )


	set(SOURCES_files
		stdneb.h
		../stdneb.cc
	)
	SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
  
  
	set(SOURCES_
		${SOURCES_sequencer}
		${SOURCES_files}
	)

  add_library( sequencer STATIC ${SOURCES_} )
  SET_TARGET_PROPERTIES(sequencer PROPERTIES FOLDER "N3SDK/addons")
    

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(sequencer stdneb.h stdneb.cc ../)
ENDIF()