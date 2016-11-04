#------------------------------------------------------------------------------
# librocket
#-------------------------------------------------------------------------------

IF(NOT WIN32)
	INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/extlibs/librocket/Rocket/Core)
ENDIF()

FILE(GLOB_RECURSE CORE_SRC "Rocket/Core/*.cpp")
FILE(GLOB_RECURSE CORE_HEAD "Rocket/Core/*.h")

SOURCE_GROUP("rocket\\core" FILES ${CORE_SRC} ${CORE_HEAD})

FILE(GLOB_RECURSE CONTROL_SRC "Rocket/Controls/*.cpp")
FILE(GLOB_RECURSE CONTROL_HEAD "Rocket/Controls/*.h")

SOURCE_GROUP("rocket\\control" FILES ${CONTROL_SRC} ${CONTROL_HEAD})

FILE(GLOB_RECURSE DBG_SRC "Rocket/Debugger/*.cpp")
FILE(GLOB_RECURSE DBG_HEAD "Rocket/Debugger/*.h")

SOURCE_GROUP("rocket\\debugger" FILES ${DBG_SRC} ${DBG_HEAD})

ADD_LIBRARY(librocket STATIC ${DBG_SRC} ${DBG_HEAD} ${CONTROL_SRC} ${CONTROL_HEAD} ${CORE_SRC} ${CORE_HEAD} Rocket/Core.h Rocket/Controls.h Rocket/Debugger.h)
TARGET_INCLUDE_DIRECTORIES(librocket PUBLIC ${Nebula3_SOURCE_DIR}/code/extlibs/librocket private ${Nebula3_SOURCE_DIR}/code/extlibs/libfreetype ${Nebula3_SOURCE_DIR}/code/foundation ${Nebula3_SOURCE_DIR}/code)
ADD_DEFINITIONS(/DSTATIC_LIB=1)
SET_TARGET_PROPERTIES(librocket PROPERTIES FOLDER "extlibs")
N_SET_COMPILER_DEFAULTS(librocket)
if(MSVC)
TARGET_COMPILE_OPTIONS(librocket PRIVATE /wd4244 /wd4101)
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(librocket precompiled.h precompiled.cpp Rocket/Core/)
ENDIF()
ENDIF()
IF(WIN32)
SET(CMAKE_CXX_FLAGS "/D__WIN32__ /DWIN32 /D_HAS_EXCEPTIONS=0 /GF /fp:fast /W1 /WX /nologo /errorReport:prompt" )
ENDIF()