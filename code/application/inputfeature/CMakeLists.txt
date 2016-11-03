#-------------------------------------------------------------------------------
# InputFeature
#-------------------------------------------------------------------------------
set(application_files_inputfeature
	inputfeatureunit.h
	inputfeatureunit.cc
)
source_group("inputfeature" FILES ${application_files_inputfeature})

set(inputfeature_precompiled_src
	../stdneb.h
	../stdneb.cc
)
set(application_inputfeature      
	${application_files_inputfeature}
	${inputfeature_precompiled_src}
)



set(input_nidls 
	
)

N_CREATE_FEATURE(inputfeature application_inputfeature input_nidls)
TARGET_INCLUDE_DIRECTORIES(inputfeature PUBLIC ${Nebula3_SOURCE_DIR}/code/application/inputfeature)
TARGET_LINK_LIBRARIES(inputfeature PUBLIC application render)
ADD_DEPENDENCIES(inputfeature application_NIDLS render_NIDLS)

N_SET_COMPILER_DEFAULTS(inputfeature)
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(inputfeature stdneb.h stdneb.cc ../)
ENDIF()