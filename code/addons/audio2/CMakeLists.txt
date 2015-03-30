#-------------------------------------------------------------------------------
# audio2
#-------------------------------------------------------------------------------


INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code
					${Nebula3_SOURCE_DIR}/code/addons/
					${Nebula3_SOURCE_DIR}/code/foundation/
					${Nebula3_SOURCE_DIR}/code/extlibs/
					${Nebula3_SOURCE_DIR}/code/extlibs/fmod/win32
					${Nebula3_SOURCE_DIR}/code/extlibs/fmoddesignerapi/win32
					${Nebula3_SOURCE_DIR}/code/render/			
					)

      set(SOURCES_files_audio2__fmodcoreaudio
        fmodcoreaudio/fmodaudiodevice.cc
        fmodcoreaudio/fmodaudiodevice.h
        fmodcoreaudio/fmodaudiohandler.cc
        fmodcoreaudio/fmodaudiohandler.h
        fmodcoreaudio/fmodcoreaudiopagehandler.cc
        fmodcoreaudio/fmodcoreaudiopagehandler.h
        fmodcoreaudio/fmoderror.cc
        fmodcoreaudio/fmoderror.h
        fmodcoreaudio/fmodeventwrapper.cc
        fmodcoreaudio/fmodeventwrapper.h
        fmodcoreaudio/fmodlistener.cc
        fmodcoreaudio/fmodlistener.h
        fmodcoreaudio/fmodmusicpromptwrapper.cc
        fmodcoreaudio/fmodmusicpromptwrapper.h
      )
      source_group("audio2\\fmodcoreaudio" FILES ${SOURCES_files_audio2__fmodcoreaudio})

      set(SOURCES_audio2__fmodcoreaudio
        ${SOURCES_files_audio2__fmodcoreaudio}
      )

    set(SOURCES_files_audio2
      audio2emitter.cc
      audio2emitter.h
      audio2interface.cc
      audio2interface.h
      audio2listener.cc
      audio2listener.h
      audio2player.cc
      audio2player.h
      audio2server.cc
      audio2server.h
      fmodeventcategoryid.h
      fmodeventhandle.h
      fmodeventid.h
      fmodeventparameterid.h
      fmodeventprojectid.h
      fmodeventstate.h
      fmodpitchunit.h
    )
    source_group("audio2" FILES ${SOURCES_files_audio2})

	set(audio2_NIDLS audio2protocol.nidl)
	
	N_WRAP_NIDL_FILES(audio2_NIDLS ${audio2_NIDLS})
	  
    set(SOURCES_audio2
      ${SOURCES_audio2__fmodcoreaudio}
      ${SOURCES_files_audio2}	  
    )

	set(SOURCES_files
            ../stdneb.h
	    ../stdneb.cc
	)
	
	SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
	set(SOURCES_SRCS
	    ${SOURCES_audio2}
	    ${SOURCES_files}
	)


  
add_library( audio2 STATIC ${SOURCES_SRCS} )
add_dependencies(audio2 audio2_NIDLS)
SET_TARGET_PROPERTIES(audio2 PROPERTIES FOLDER "N3SDK/addons")

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(audio2 stdneb.h stdneb.cc ../)
ENDIF()
  
