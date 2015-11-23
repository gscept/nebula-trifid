#-------------------------------------------------------------------------------
# anyfx api
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(${ANYFX_SOURCE_DIR}
					${ANYFX_SOURCE_DIR}/util/code
					${ANYFX_GLEW_FOLDER}/include
					${ANYFX_SOURCE_DIR}/exts/eastl/include
					${ANYFX_SOURCE_DIR}/api/code)
					

FILE(GLOB SRC "code/*.cc")
FILE(GLOB HEAD "code/*.h")
SOURCE_GROUP("api" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/internal/*.cc")
FILE(GLOB HEAD "code/internal/*.h")
SOURCE_GROUP("api\\internal" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/internal/glsl4/*.cc")
FILE(GLOB HEAD "code/internal/glsl4/*.h")
SOURCE_GROUP("api\\internal\\glsl4" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/loaders/*.cc")
FILE(GLOB HEAD "code/loaders/*.h")
SOURCE_GROUP("api\\loaders" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

#FILE(GLOB SRC "glew/*.c")
FILE(GLOB HEAD "GL/*.h")
SOURCE_GROUP("api\\glew" FILES ${HEAD})
LIST(APPEND CODE ${HEAD})

ADD_DEFINITIONS(-DGLEW_STATIC)

IF(ANYFX_TRANSPOSE_MATRICES)
	ADD_DEFINITIONS(-D__ANYFX_TRANSPOSE_MATRIX__)
ENDIF()

IF (MSVC11)
	ADD_DEFINITIONS(-D_XKEYCHECK_H)
ENDIF()

ADD_LIBRARY(api STATIC ${CODE})

ADD_DEPENDENCIES(api util)
TARGET_LINK_LIBRARIES(api util)
