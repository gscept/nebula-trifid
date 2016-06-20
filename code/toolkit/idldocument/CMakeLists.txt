#-------------------------------------------------------------------------------
# idldocument
#-------------------------------------------------------------------------------

FILE(GLOB SRC "idldocument/*.cc")
	FILE(GLOB HEAD "idldocument/*.h")
	LIST(APPEND SRCSIDL ${SRC} ${HEAD})
	SOURCE_GROUP("idldocument" FILES ${SRCSIDL})
	
set(PCH_files	
	stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})
ADD_LIBRARY(idldocument ${SRCSIDL} ${PCH_files})
SET_TARGET_PROPERTIES(idldocument PROPERTIES FOLDER "N3SDK/Toolkit")
TARGET_INCLUDE_DIRECTORIES(idldocument PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

N_SET_COMPILER_DEFAULTS()

ADD_DEPENDENCIES(idldocument foundation)
TARGET_LINK_LIBRARIES(idldocument PUBLIC foundation)

