#-------------------------------------------------------------------------------
# effects
#-------------------------------------------------------------------------------

set(SOURCES_files_effects
	effectsfeatureunit.cc
	effectsfeatureunit.h
	animeventregistry.cc
	animeventregistry.h
)
source_group("effects" FILES ${SOURCES_files_effects})

set(SOURCES_effects__base
	${SOURCES_files_effects}
)

set(SOURCES_files_effects__properties
properties/effectproperty.cc
properties/effectproperty.h
)
source_group("effects\\properties" FILES ${SOURCES_files_effects__properties})

set(SOURCES_effects__properties
${SOURCES_files_effects__properties}
)

set(SOURCES_files_effects__effects
	effects/attachmenteffect.cc
	effects/attachmenteffect.h
	effects/cameraanimationeffect.cc
	effects/cameraanimationeffect.h
	effects/camerashakeeffect.cc
	effects/camerashakeeffect.h
	effects/effect.cc
	effects/effect.h
	effects/postfxeffect.cc
	effects/postfxeffect.h
	effects/vibrationeffect.cc
	effects/vibrationeffect.h
	effects/audioeffect.cc
	effects/audioeffect.h
	effects/graphicseffect.cc
	effects/graphicseffect.h
)
source_group("effects\\effects" FILES ${SOURCES_files_effects__effects})

set(SOURCES_effects__effects
	${SOURCES_files_effects__effects}
)

set(SOURCES_files_effects__animeventhandler
	animeventhandler/animeventattachmenthandler.cc
	animeventhandler/animeventattachmenthandler.h
	animeventhandler/animeventshakehandler.cc
	animeventhandler/animeventshakehandler.h
	animeventhandler/animeventsoundhandler.cc
	animeventhandler/animeventsoundhandler.h
	animeventhandler/animeventtiminghandler.cc
	animeventhandler/animeventtiminghandler.h
	animeventhandler/animeventvibrationhandler.cc
	animeventhandler/animeventvibrationhandler.h
)
source_group("effects\\animeventhandler" FILES ${SOURCES_files_effects__animeventhandler})

set(SOURCES_effects__animeventhandler
	${SOURCES_files_effects__animeventhandler}
)

set(SOURCES_files_effects__mixer
	mixer/camerashakemixer.cc
	mixer/camerashakemixer.h
	mixer/effectmixer.cc
	mixer/effectmixer.h
)
source_group("effects\\mixer" FILES ${SOURCES_files_effects__mixer})

set(SOURCES_effects__mixer
	${SOURCES_files_effects__mixer}
)

set(SOURCES_effects
	${SOURCES_effects__core}
	${SOURCES_effects__effects}
	${SOURCES_effects__animeventhandler}
	${SOURCES_effects__mixer}
	${SOURCES_effects__properties}
	${SOURCES_files_effects}
)

set(SOURCES_files
	../stdneb.h
	../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
set(P_NIDL_FILES effectsattributes.nidl effectscommands.nidl)
	
set(SOURCES_SRCS
	${SOURCES_effects}
	${SOURCES_files}		
)

N_WRAP_ADD_NIDL_FILES(SOURCES_SRCS ${P_NIDL_FILES})

add_library( effects STATIC ${SOURCES_SRCS} )
TARGET_LINK_LIBRARIES(effects ${FMOD_LIBRARIES} application)
ADD_DEPENDENCIES(effects application_NIDLS)

SET_TARGET_PROPERTIES(effects PROPERTIES FOLDER "N3SDK/addons")

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(effects stdneb.h stdneb.cc ../)
ENDIF()
 
