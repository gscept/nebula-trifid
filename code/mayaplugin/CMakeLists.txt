#-------------------------------------------------------------------------------
# Maya Plugin
#-------------------------------------------------------------------------------

SET(MAYALIBPATH ${Nebula3_SOURCE_DIR}/extlibs/maya/lib)
SET(MAYALIBS ${MAYALIBPATH}/Foundation.lib; ${MAYALIBPATH}/OpenMaya.lib; ${MAYALIBPATH}/OpenMayaAnim.lib;)

INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}
					${Nebula3_SOURCE_DIR}/foundation/
                    ${Nebula3_SOURCE_DIR}/extlibs/
					${Nebula3_SOURCE_DIR}/render/
					${Nebula3_SOURCE_DIR}/mayaplugin/
					${Nebula3_SOURCE_DIR}/extlibs/maya/include
					)

FILE(GLOB SRC "mayacore/*.cc")
FILE(GLOB HEAD "mayacore/*.h")
LIST(APPEND CORE ${SRC} ${HEAD})
SOURCE_GROUP("mayaplugin\\mayacore" FILES ${CORE})

FILE(GLOB SRC "mayacommands/*.cc")
FILE(GLOB HEAD "mayacommands/*.h")
LIST(APPEND COMS ${SRC} ${HEAD})
SOURCE_GROUP("mayaplugin\\mayacommands" FILES ${COMS})

FILE(GLOB SRC "mayaexporters/*.cc")
FILE(GLOB HEAD "mayaexporters/*.h")
LIST(APPEND EXPORTERS ${SRC} ${HEAD})
SOURCE_GROUP("mayaplugin\\mayaexporters" FILES ${EXPORTERS})

FILE(GLOB SRC "mayaextractors/*.cc")
FILE(GLOB HEAD "mayaextractors/*.h")
LIST(APPEND EXTRACTORS ${SRC} ${HEAD})
SOURCE_GROUP("mayaplugin\\mayaextractors" FILES ${EXTRACTORS})

FILE(GLOB SRC "mayalegacy/*.cc")
FILE(GLOB HEAD "mayalegacy/*.h")
LIST(APPEND LEGACY ${SRC} ${HEAD})
SOURCE_GROUP("mayaplugin\\mayalegacy" FILES ${LEGACY})

FILE(GLOB SRC "mayaloadsave/*.cc")
FILE(GLOB HEAD "mayaloadsave/*.h")
LIST(APPEND LOADSAVE ${SRC} ${HEAD})
SOURCE_GROUP("mayaplugin\\mayaloadsave" FILES ${LOADSAVE})

FILE(GLOB SRC "mayamain/*.cc")
FILE(GLOB HEAD "mayamain/*.h")
LIST(APPEND MAIN ${SRC} ${HEAD})
SOURCE_GROUP("mayaplugin\\mayamain" FILES ${MAIN})

FILE(GLOB SRC "mayashaders/*.cc")
FILE(GLOB HEAD "mayashaders/*.h")
LIST(APPEND SHADERS ${SRC} ${HEAD})
SOURCE_GROUP("mayaplugin\\mayashaders" FILES ${SHADERS})

FILE(GLOB SRC "mayautil/*.cc")
FILE(GLOB HEAD "mayautil/*.h")
LIST(APPEND UTIL ${SRC} ${HEAD})
SOURCE_GROUP("mayaplugin\\mayautil" FILES ${UTIL})


set(PCH_files	
    stdneb.cc
)

SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${CORE} ${COMS} ${EXPORTERS} ${EXTRACTORS} ${LEGACY} ${LOADSAVE} ${MAIN} ${SHADERS} ${UTIL} ${PCH_files})

ADD_LIBRARY(mayaplugin SHARED ${ALLSRCS} )
SET_TARGET_PROPERTIES(mayaplugin PROPERTIES FOLDER "N3SDK/toolkit")
SET_TARGET_PROPERTIES(mayaplugin PROPERTIES SUFFIX ".mll")
SET_TARGET_PROPERTIES(mayaplugin PROPERTIES OUTPUT_NAME "Nebula3Plugin")

#-- definition needed for maya to compile
ADD_DEFINITIONS(-D_BOOL -D__MAYA__ -DNT_PLUGIN -D_AFXDLL)

N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(mayaplugin)
N_SET_EXE_SUBSYSTEM(mayaplugin Windows)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(mayaplugin stdneb.h stdneb.cc)
ENDIF()

LINK_DIRECTORIES(${N_LIBDIRS}
				 ${MAYALIBPATH}
	)
ADD_DEPENDENCIES(mayaplugin
		foundation
		zlib		
		render
		toolkitutil
		tinyxml		
	)
TARGET_LINK_LIBRARIES(mayaplugin
		foundation
		zlib		
		render		
		toolkitutil
		tinyxml
		${N_EXTRA_WIN32_LIBS}
		${MAYALIBS}
	)