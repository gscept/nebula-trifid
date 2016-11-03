#-------------------------------------------------------------------------------
# StateObjectFeature
#-------------------------------------------------------------------------------


   

set(application_files_stateobjectfeature__properties
	properties/stategraphicsproperty.cc
	properties/stategraphicsproperty.h
	properties/stateinfo.h
	properties/stateproperty.cc
	properties/stateproperty.h
)
source_group("stateobjectfeature\\properties" FILES ${application_files_stateobjectfeature__properties})

set(application_stateobjectfeature__properties
	${application_files_stateobjectfeature__properties}
)

set(application_files_stateobjectfeature
	stateobjectfeatureunit.h
)
source_group("stateobjectfeature" FILES ${application_files_stateobjectfeature})

set(_precompiled_src
	../stdneb.h
	../stdneb.cc
)
set(feature_sources      
	${application_stateobjectfeature__properties}
	${application_files_stateobjectfeature}
	${_precompiled_src}
)

set(feature_nidls stateobjectprotocol.nidl)	

N_CREATE_FEATURE(stateobjectfeature feature_sources feature_nidls)
TARGET_INCLUDE_DIRECTORIES(stateobjectfeature PUBLIC ${Nebula3_SOURCE_DIR}/code/application/stateobjectfeature)
TARGET_LINK_LIBRARIES(stateobjectfeature PUBLIC application graphicsfeature)
ADD_DEPENDENCIES(stateobjectfeature application graphicsfeature)

N_SET_COMPILER_DEFAULTS(stateobjectfeature)
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(stateobjectfeature stdneb.h stdneb.cc ../)
ENDIF()