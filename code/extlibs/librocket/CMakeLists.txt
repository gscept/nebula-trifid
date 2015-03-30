#------------------------------------------------------------------------------
# librocket
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/extlibs/librocket ${Nebula3_SOURCE_DIR}/code/extlibs/libfreetype ${Nebula3_SOURCE_DIR}/code/foundation ${Nebula3_SOURCE_DIR}/code )
IF(NOT WIN32)
	INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/extlibs/librocket/Rocket/Core)
ENDIF()

FILE(GLOB_RECURSE SRC "Rocket/*.cpp")
FILE(GLOB_RECURSE HEAD "Rocket/*.h")
ADD_LIBRARY(librocket STATIC ${SRC} ${HEAD})

ADD_DEFINITIONS(/DSTATIC_LIB=1)
SET_TARGET_PROPERTIES(librocket PROPERTIES FOLDER "extlibs")
N_SET_COMPILER_DEFAULTS()
if(MSVC)
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(librocket precompiled.h precompiled.cpp Rocket/Core/)
ENDIF()
ENDIF()
IF(WIN32)
SET(CMAKE_CXX_FLAGS "/D__WIN32__ /DWIN32 /D_HAS_EXCEPTIONS=0 /GF /fp:fast /W1 /WX /nologo /errorReport:prompt" )
ENDIF()