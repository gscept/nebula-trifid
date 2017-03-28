#-------------------------------------------------------------------------------
# PhysicsFeature
#-------------------------------------------------------------------------------


set(application_files_physicsfeature__properties
	properties/actorphysicsproperty.cc
	properties/actorphysicsproperty.h
	properties/animatedphysicsproperty.cc
	properties/animatedphysicsproperty.h
	properties/environmentcollideproperty.cc
	properties/environmentcollideproperty.h
	properties/mousegripperproperty.cc
	properties/mousegripperproperty.h
	properties/physicsproperty.cc
	properties/physicsproperty.h        
	properties/staticphysicsproperty.cc
	properties/staticphysicsproperty.h        
	properties/triggerproperty.cc
	properties/triggerproperty.h
	properties/pointnclickinputproperty.cc
	properties/pointnclickinputproperty.h
	properties/interactproperty.cc
	properties/interactproperty.h        
)
source_group("physicsfeature\\properties" FILES ${application_files_physicsfeature__properties})

set(application_physicsfeature__properties
	${application_files_physicsfeature__properties}
)

set(application_files_physicsfeature
	physicsfeatureunit.cc
	physicsfeatureunit.h
	gamefilterset.cc
	gamefilterset.h
)
source_group("physicsfeature" FILES ${application_files_physicsfeature})

set(physicsfeature_precompiled_src
	../stdneb.h
	../stdneb.cc
)
set(feature_sources      
	${application_physicsfeature__properties}
	${application_files_physicsfeature}
	${physicsfeature_precompiled_src}
)

set(feature_nidls 
		
)

N_CREATE_FEATURE(physicsfeature feature_sources feature_nidls)
TARGET_INCLUDE_DIRECTORIES(physicsfeature PUBLIC ${Nebula3_SOURCE_DIR}/code/application/physicsfeature)
TARGET_LINK_LIBRARIES(physicsfeature PUBLIC application render physics navigation)
ADD_DEPENDENCIES(physicsfeature application_NIDLS)

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(physicsfeature stdneb.h stdneb.cc ../)
ENDIF()