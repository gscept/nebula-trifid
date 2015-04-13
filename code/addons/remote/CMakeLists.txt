#-------------------------------------------------------------------------------
# remote
#-------------------------------------------------------------------------------



INCLUDE_DIRECTORIES(
					${Nebula3_SOURCE_DIR}/code/addons/
					${Nebula3_SOURCE_DIR}/code/foundation/
					${Nebula3_SOURCE_DIR}/code/extlibs/
					)

					
    set(SOURCES_files_remote
      defaultremoterequesthandler.cc
      defaultremoterequesthandler.h
      remotecommandhandler.cc
      remotecommandhandler.h
      remotecontrol.cc
      remotecontrol.h
      remotecontrolproxy.cc
      remotecontrolproxy.h
      remoteinterface.cc
      remoteinterface.h
      remotemessagehandler.cc
      remotemessagehandler.h      
      remoterequest.cc
      remoterequest.h
      remoterequesthandler.cc
      remoterequesthandler.h
      remoterequestreader.cc
      remoterequestreader.h
      remoteresponsewriter.cc
      remoteresponsewriter.h
      remotestatus.cc
      remotestatus.h
    )
    source_group("remote" FILES ${SOURCES_files_remote})

    set(SOURCES_remote
      ${SOURCES_files_remote}
    )


	set(SOURCES_files
		../stdneb.h
		../stdneb.cc
	)
	SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
  
	set(P_NIDL_FILES remoteprotocol.nidl)
  
	
  
	set(SOURCES_
		${SOURCES_remote}
		${SOURCES_files}		
	)
	N_WRAP_ADD_NIDL_FILES(SOURCES_ ${P_NIDL_FILES})

  add_library( remote STATIC ${SOURCES_} )
  add_dependencies(remote foundation)
  SET_TARGET_PROPERTIES(remote PROPERTIES FOLDER "N3SDK/addons")
    

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(remote stdneb.h stdneb.cc ../)
ENDIF()
