#-------------------------------------------------------------------------------
# anyfx compiler
#-------------------------------------------------------------------------------


ADD_SUBDIRECTORY(antlr)
ADD_SUBDIRECTORY(mcpp)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
		    ${ANYFX_SOURCE_DIR}/util/code
		    ${ANYFX_SOURCE_DIR}/exts/glew
		    ${ANYFX_SOURCE_DIR}/compiler/code
		    ${ANYFX_SOURCE_DIR}/compiler/antlr/include
		    ${ANYFX_SOURCE_DIR}/compiler/mcpp
)
					

FILE(GLOB SRC "code/*.cc")
FILE(GLOB HEAD "code/*.h")
SOURCE_GROUP("compiler" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "code/expressions/*.cc")
FILE(GLOB HEAD "code/expressions/*.h")
SOURCE_GROUP("compiler\\expressions" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC "../exts/glew/*.c")
FILE(GLOB HEAD "../exts/glew/GL/*.h")
SOURCE_GROUP("compiler\\glew" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})
ADD_DEFINITIONS(-DGLEW_STATIC)

FILE(GLOB SRC code/lexererrorhandler.cc code/parsererrorhandler.cc)
FILE(GLOB HEAD code/lexererrorhandler.h code/parsererrorhandler.h)
SOURCE_GROUP("parser" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

FILE(GLOB SRC code/parser/AnyFXParser.c code/parser/AnyFXLexer.c)
FILE(GLOB HEAD code/parser/AnyFXParser.h code/parser/AnyFXLexer.h)
SOURCE_GROUP("parser" FILES ${SRC} ${HEAD})
set_source_files_properties( ${SRC} PROPERTIES LANGUAGE "CXX" )
LIST(APPEND CODE ${SRC} ${HEAD})


IF(ANYFX_BUILD_GRAMMAR)
	FILE(GLOB GRAMMAR "antlr/grammar/AnyFX.g")
	SOURCE_GROUP("grammar" FILES ${GRAMMAR})
	LIST(APPEND CODE ${GRAMMAR})

	SET_DIRECTORY_PROPERTIES(PROPERTIES 
		DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/code/parser PROPERTY ADDITIONAL_MAKE_CLEAN_FILES)

	ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/code/parser
				COMMAND java -jar ${CMAKE_CURRENT_SOURCE_DIR}/antlr/antlr.jar ${GRAMMAR} -fo ${CMAKE_CURRENT_SOURCE_DIR}/code/parser
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				DEPENDS ${GRAMMAR}
	)

	ADD_CUSTOM_TARGET(GenerateParser ALL
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/code/parser)
ENDIF()
	
IF (ANYFX_BUILD_COMPILER_LIBRARY)

	# remove main from cmake
	FILE(GLOB MAIN "code/main.cc")
	LIST(REMOVE_ITEM CODE ${MAIN})
	
	ADD_DEFINITIONS(-D__ANYFX_COMPILER_LIBRARY__)
	ADD_LIBRARY(compiler STATIC ${CODE})
ELSE()
	ADD_EXECUTABLE(compiler ${CODE})	
ENDIF()

	ADD_DEPENDENCIES(compiler
		#GenerateParser
		util
		antlr
		mcpp)
		
	TARGET_LINK_LIBRARIES(compiler
		util
		antlr
		mcpp
		${OPENGL_LIBS})
		
SET_DEFAULT_OUTPUT(compiler)

IF (MSVC11)
	ADD_DEFINITIONS(-D_XKEYCHECK_H)
ENDIF()
