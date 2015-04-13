#-------------------------------------------------------------------------------
# base64
#-------------------------------------------------------------------------------

SET(base64_SOURCES
	base64.h
	base64.c
)

SOURCE_GROUP(base64 FILES ${base64_SOURCES})

ADD_LIBRARY(base64 STATIC ${base64_SOURCES})
TARGET_INCLUDE_DIRECTORIES(base64 PUBLIC ${Nebula3_SOURCE_DIR}/code/extlibs/base64)
SET_TARGET_PROPERTIES(base64 PROPERTIES FOLDER "extlibs")

N_SET_COMPILER_DEFAULTS()
