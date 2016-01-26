#-------------------------------------------------------------------------------
# ScriptingFeature
#-------------------------------------------------------------------------------


   
set(scripting_files
	scriptingfeature.cc
	scriptingfeature.h
	scriptinginputhandler.cc
	scriptinginputhandler.h
	properties/scriptingproperty.cc
	properties/scriptingproperty.h
	properties/scriptingtriggerproperty.cc
	properties/scriptingtriggerproperty.h
)
source_group("scriptingfeature" FILES ${scripting_files})
	


set(_precompiled_src
	../stdneb.h
	../stdneb.cc
)
set(feature_sources      
	${scripting_files}	
	${_precompiled_src}
)

set(feature_nidls 
	scriptingcommands.nidl 
)

N_CREATE_FEATURE(scriptingfeature feature_sources feature_nidls)
TARGET_INCLUDE_DIRECTORIES(scriptingfeature PUBLIC ${Nebula3_SOURCE_DIR}/code/application/scriptingfeature)
TARGET_LINK_LIBRARIES(scriptingfeature PUBLIC application scripting graphicsfeature)
ADD_DEPENDENCIES(scriptingfeature application physicsfeature)

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(scriptingfeature stdneb.h stdneb.cc ../)
ENDIF()