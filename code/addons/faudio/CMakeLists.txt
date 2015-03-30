#-------------------------------------------------------------------------------
# faudio
#-------------------------------------------------------------------------------


INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code
					${Nebula3_SOURCE_DIR}/code/addons/
					${Nebula3_SOURCE_DIR}/code/foundation/
					${Nebula3_SOURCE_DIR}/code/application/
					${Nebula3_SOURCE_DIR}/code/render/
					${Nebula3_SOURCE_DIR}/code/extlibs/			
					)

    

    set(SOURCES_files_faudio
		audiointerface.cc
		audiointerface.h
		audiolistener.cc
		audiolistener.h		
		audioserver.cc
		audioserver.h
		fmodstudiopagehandler.cc
		fmodstudiopagehandler.h
		fmoderror.cc
		fmoderror.h
		eventid.h
		eventinstance.cc
		eventinstance.h
		eventstate.h
		eventparameterid.h
		busid.h
		audiohandler.cc
		audiohandler.h
		audiodevice.cc
		audiodevice.h
		audioemitterproperty.cc
		audioemitterproperty.h
    )
    source_group("audio" FILES ${SOURCES_files_audio})

	set(faudio_NIDLS audioprotocol.nidl)
	
	N_WRAP_NIDL_FILES(faudio_NIDLS ${faudio_NIDLS})
	  
    set(SOURCES_faudio      
      ${SOURCES_files_faudio}	  
    )

	set(SOURCES_files
            ../stdneb.h
	    ../stdneb.cc
	)
	
	SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
	set(SOURCES_SRCS
	    ${SOURCES_faudio}
	    ${SOURCES_files}
	)


  
add_library( faudio STATIC ${SOURCES_SRCS} )
add_dependencies(faudio faudio_NIDLS)
SET_TARGET_PROPERTIES(faudio PROPERTIES FOLDER "N3SDK/addons")

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(faudio stdneb.h stdneb.cc ../)
ENDIF()
  

  
