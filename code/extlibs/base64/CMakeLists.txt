#-------------------------------------------------------------------------------
# base64
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(    
    ${Nebula3_SOURCE_DIR}/code/foundation/
    ${Nebula3_SOURCE_DIR}/code/extlibs/base64
)

SET(base64_SOURCES
	base64.h
	base64.c
)

SOURCE_GROUP(base64 FILES ${base64_SOURCES})

ADD_LIBRARY(base64 STATIC ${base64_SOURCES})
SET_TARGET_PROPERTIES(base64 PROPERTIES FOLDER "extlibs")

N_SET_COMPILER_DEFAULTS()
