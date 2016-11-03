#-------------------------------------------------------------------------------
# SOIL
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/extlibs/SOIL/original
				    ${Nebula3_SOURCE_DIR}/extlibs/SOIL
					)
					
					
FILE(GLOB HEAD "*.h")
FILE(GLOB SRC "*.c")
LIST(APPEND CODE ${HEAD} ${SRC})

#FILE(GLOB HEAD "original/*.h")
#FILE(GLOB SRC "original/*.c")
#LIST(APPEND CODE ${HEAD} ${SRC})

SOURCE_GROUP(SOIL FILES ${CODE})

ADD_LIBRARY(SOIL STATIC ${CODE})
SET_TARGET_PROPERTIES(SOIL PROPERTIES FOLDER "extlibs")

N_SET_COMPILER_DEFAULTS(SOIL)

IF(WIN32)
	SET(CMAKE_C_FLAGS "/D__WIN32__ /DWIN32 /D_HAS_EXCEPTIONS=0 /GF /fp:fast /W0 /nologo /errorReport:prompt" )
ELSEIF(UNIX)

ENDIF()
