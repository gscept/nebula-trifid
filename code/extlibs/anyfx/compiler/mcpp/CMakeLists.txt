#-------------------------------------------------------------------------------
# mcpp
#-------------------------------------------------------------------------------


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

FILE(GLOB SRC "*.c")
FILE(GLOB HEAD1 "*.h")
FILE(GLOB HEAD2 "*.H")
SOURCE_GROUP("mcpp" FILES ${SRC} ${HEAD1} ${HEAD2})
LIST(APPEND CODE ${SRC} ${HEAD1} ${HEAD2})

IF(MSVC)
	ADD_DEFINITIONS("/W0")
ELSEIF(UNIX OR APPLE)
	ADD_DEFINITIONS("-w -DUNIX")
ENDIF()
ADD_DEFINITIONS(-DMCPP_LIB)

ADD_LIBRARY(mcpp STATIC ${CODE})
