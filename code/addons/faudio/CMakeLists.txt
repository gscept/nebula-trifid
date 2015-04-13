#-------------------------------------------------------------------------------
# faudio
#-------------------------------------------------------------------------------

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

N_WRAP_ADD_NIDL_FILES(SOURCES_SRCS ${faudio_NIDLS})
 
FIND_PACKAGE(FmodStudio REQUIRED)

add_library( faudio STATIC ${SOURCES_SRCS} )
TARGET_LINK_LIBRARIES(faudio ${FMOD_LIBRARIES} application)
TARGET_INCLUDE_DIRECTORIES(faudio PUBLIC ${FMOD_INCLUDE_DIRS})
add_dependencies(faudio application)
SET_TARGET_PROPERTIES(faudio PROPERTIES FOLDER "N3SDK/addons")

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(faudio stdneb.h stdneb.cc ../)
ENDIF()
  

  
