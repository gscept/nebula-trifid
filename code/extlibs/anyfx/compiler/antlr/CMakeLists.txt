#-------------------------------------------------------------------------------
# antlr
#-------------------------------------------------------------------------------


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
					include)

FILE(GLOB SRC "src/*.c")
FILE(GLOB HEAD "include/*.h")
SOURCE_GROUP("antlr" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

ADD_LIBRARY(antlr STATIC ${CODE})
