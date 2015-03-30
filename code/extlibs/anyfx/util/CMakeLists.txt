#-------------------------------------------------------------------------------
# anyfx util
#-------------------------------------------------------------------------------

PROJECT(util)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
					${ANYFX_SOURCE_DIR}/util/code)
					

FILE(GLOB SRC "code/*.cc")
FILE(GLOB HEAD "code/*.h")
SOURCE_GROUP("util" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})


ADD_LIBRARY(util STATIC ${CODE})



