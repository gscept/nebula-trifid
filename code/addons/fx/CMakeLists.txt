#-------------------------------------------------------------------------------
# fx
#-------------------------------------------------------------------------------



INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}
					${Nebula3_SOURCE_DIR}/addons/
					${Nebula3_SOURCE_DIR}/foundation/
					${Nebula3_SOURCE_DIR}/extlibs/
					${Nebula3_SOURCE_DIR}/render/	
					${Nebula3_SOURCE_DIR}/application
					${Nebula3_SOURCE_DIR}/application/basegamefeature					
					${Nebula3_SOURCE_DIR}
					)


      set(SOURCES_files_fx__fxfeature
        fxfeature/fxfeatureunit.cc
        fxfeature/fxfeatureunit.h
        fxfeature/fxmanager.cc
        fxfeature/fxmanager.h
		fxfeature/fxrendermodule.cc
		fxfeature/fxrendermodule.h
      )
      source_group("fx\\fxfeature" FILES ${SOURCES_files_fx__fxfeature})

      set(SOURCES_fx__fxfeature
        ${SOURCES_files_fx__fxfeature}
      )

      set(SOURCES_files_fx__fxproperties
        fxproperties/effectproperty.cc
        fxproperties/effectproperty.h
      )
      source_group("fx\\fxproperties" FILES ${SOURCES_files_fx__fxproperties})

      set(SOURCES_fx__fxproperties
        ${SOURCES_files_fx__fxproperties}
      )

        set(SOURCES_files_fx__rt__effects
          rt/effects/cameraanimationeffect.cc
          rt/effects/cameraanimationeffect.h
          rt/effects/camerashakeeffect.cc
          rt/effects/camerashakeeffect.h
          rt/effects/effect.cc
          rt/effects/effect.h
          rt/effects/vibrationeffect.cc
          rt/effects/vibrationeffect.h
		  rt/effects/audioeffect.cc
		  rt/effects/audioeffect.h
		  rt/effects/graphicseffect.cc
		  rt/effects/graphicseffect.h
        )
        source_group("fx\\rt\\effects" FILES ${SOURCES_files_fx__rt__effects})

        set(SOURCES_fx__rt__effects
          ${SOURCES_files_fx__rt__effects}
        )

        set(SOURCES_files_fx__rt__fxanimeventhandler
          rt/fxanimeventhandler/animeventattachmenthandler.cc
          rt/fxanimeventhandler/animeventattachmenthandler.h
          rt/fxanimeventhandler/animeventshakehandler.cc
          rt/fxanimeventhandler/animeventshakehandler.h
          rt/fxanimeventhandler/animeventsoundhandler.cc
          rt/fxanimeventhandler/animeventsoundhandler.h
          rt/fxanimeventhandler/animeventtiminghandler.cc
          rt/fxanimeventhandler/animeventtiminghandler.h
          rt/fxanimeventhandler/animeventvibrationhandler.cc
          rt/fxanimeventhandler/animeventvibrationhandler.h
        )
        source_group("fx\\rt\\fxanimeventhandler" FILES ${SOURCES_files_fx__rt__fxanimeventhandler})

        set(SOURCES_fx__rt__fxanimeventhandler
          ${SOURCES_files_fx__rt__fxanimeventhandler}
        )

        set(SOURCES_files_fx__rt__mixer
          rt/mixer/camerashakemixer.cc
          rt/mixer/camerashakemixer.h
          rt/mixer/effectmixer.cc
          rt/mixer/effectmixer.h
        )
        source_group("fx\\rt\\mixer" FILES ${SOURCES_files_fx__rt__mixer})

        set(SOURCES_fx__rt__mixer
          ${SOURCES_files_fx__rt__mixer}
        )

      set(SOURCES_files_fx__rt
        rt/fxhandler.cc
        rt/fxhandler.h
		rt/fxmessagehandler.cc
        rt/fxrtplugin.cc
        rt/fxrtplugin.h
        rt/fxserver.cc
        rt/fxserver.h
		rt/animeventregistry.cc
		rt/animeventregistry.h
      )
      source_group("fx\\rt" FILES ${SOURCES_files_fx__rt})

      set(SOURCES_fx__rt
        ${SOURCES_fx__rt__effects}
        ${SOURCES_fx__rt__fxanimeventhandler}
        ${SOURCES_fx__rt__mixer}
        ${SOURCES_files_fx__rt}
      )

    set(SOURCES_fx
      ${SOURCES_fx__fxfeature}
      ${SOURCES_fx__fxproperties}
      ${SOURCES_fx__rt}
    )

	set(SOURCES_files
		stdneb.h
		../stdneb.cc
	)
	SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
	set(P_NIDL_FILES fxprotocol.nidl)

	N_WRAP_NIDL_FILES(fx_NIDLS ${P_NIDL_FILES})

	set(SOURCES_SRCS
		${SOURCES_fx}
		${SOURCES_files}
		${SOURCES_NIDLS}
	)

add_library( fx STATIC ${SOURCES_SRCS} )

ADD_DEPENDENCIES(fx application)

SET_TARGET_PROPERTIES(fx PROPERTIES FOLDER "N3SDK/addons")

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(fx stdneb.h stdneb.cc ../)
ENDIF()
 