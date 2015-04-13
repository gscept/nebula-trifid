set_property(GLOBAL PROPERTY USE_FOLDERS On)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0.2)

PROJECT(Nebula3)

include(CMakeDependentOption)

SET(N3ROOT ${Nebula3_SOURCE_DIR})

# for makefile based builds default to release builds
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

#-------------------------------------------------------------------------------
# Settings that can be tweaked by the user prior to building.
#-------------------------------------------------------------------------------

OPTION(N_USE_PRECOMPILED_HEADERS "Use precompiled headers." ON)

SET(N_MATH_XNA OFF)
SET(N_MATH_VECMATH OFF)

SET(DEFMATH "N_MATH_VECMATH")

SET(N_MATH ${DEFMATH} CACHE STRING "Math library used")
SET_PROPERTY(CACHE N_MATH PROPERTY STRINGS "N_MATH_XNA" "N_MATH_VECMATH")
SET(${N_MATH} ON)

IF(N_MATH_XNA)
	ADD_DEFINITIONS(-D__USE_XNA)
ELSE()
	ADD_DEFINITIONS(-D__USE_VECMATH)
	INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/extlibs/bullet/bullet/src)
ENDIF()




OPTION(N_BUILD_OLD "Build old mostly obsolete things (some probably broken)." OFF)
OPTION(N_BUILD_TESTS "Build Tests." OFF)
OPTION(N_BUILD_TOOLKIT "Build Toolkit." ON)
OPTION(N_BUILD_BENCHMARKS "Build Benchmarks (probably broken)." OFF)
OPTION(N_BUILD_PLUGIN "Build Plugin." OFF)
OPTION(N_BUILD_QT "Build QT based toolkit programs." ON)
OPTION(N_BUILD_NVTT "Build nVidia texture tools and use as texture exporter." ON) 
OPTION(N_BUILD_FIRSTTIME "Build Nebula without foundation nidls (required for first-time compilation of idlcompiler)." OFF)
OPTION(N_BUILD_PUBLIC_AS_RELEASE "Instead of outputting release build as name.public.exe they are called name.exe." OFF)
CMAKE_DEPENDENT_OPTION(N_QT_NEB_SUPPORT "Add support for Qt features within Nebula" ON "N_BUILD_QT" OFF)
OPTION(N_BUILD_EXTERNAL_PROJECTS "Include projects included in the projects folder" ON)

IF(!MSVC) 
	SET(N_BUILD_NVTT ON)
ENDIF()

IF(N_BUILD_DX11 AND N_BUILD_OGL4)
	MESSAGE(FATAL_ERROR "Both DirectX 11 and OpenGL4 can't be picked as renderers")
ENDIF()

IF(NOT CMAKE_MAJOR_VERSION VERSION_LESS 3)
# disable warnings related to trying to perform ADD_DEFINITIONS on unknown libs
CMAKE_POLICY(SET CMP0046 OLD)
ENDIF()


# select render api

SET(N_BUILD_DX11 OFF)
SET(N_BUILD_OGL4 OFF)
SET(N_BUILD_DX9 OFF)

SET(DEFAULTRENDERER "N_BUILD_OGL4")

SET(N_RENDERER ${DEFAULTRENDERER} CACHE STRING "Renderer chosen by CMake based on operating system")
SET_PROPERTY(CACHE N_RENDERER PROPERTY STRINGS "N_BUILD_DX11" "N_BUILD_OGL4" "N_BUILD_DX9")
SET(${N_RENDERER} ON)
# these are required for all extlibs packages that use precompiled headers from nebula
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS  $<$<BOOL:${N_BUILD_OGL4}>:__OGL4__ GLEW_STATIC __ANYFX__ IL_STATIC_LIB> $<$<BOOL:${N_BUILD_DX11}>:__DX11__> $<$<BOOL:${N_BUILD_DX9}>:__DX9__>)
INCLUDE_DIRECTORIES($<$<BOOL:${N_BUILD_OGL4}>:${Nebula3_SOURCE_DIR}/code/extlibs/anyfx/api/code
												${Nebula3_SOURCE_DIR}/code/extlibs/glew/include
												${Nebula3_SOURCE_DIR}/code/extlibs/anyfx/api/code
												${Nebula3_SOURCE_DIR}/code/extlibs/anyfx/util/code
												${Nebula3_SOURCE_DIR}/code/extlibs/anyfx/compiler/code
												${Nebula3_SOURCE_DIR}/code/extlibs/anyfx/exts/eastl/include
												${Nebula3_SOURCE_DIR}/code/extlibs/glfw/include>
												$<$<BOOL:${N_BUILD_DX11}>:$ENV{DXSDK_DIR}/include ${Nebula3_SOURCE_DIR}/code/extlibs/Effects11> $<$<BOOL:${N_BUILD_DX9}>:$ENV{DXSDK_DIR}/include>)


# select physics implementation
SET(N_BUILD_BULLET OFF)
SET(N_BUILD_HAVOK OFF)
SET(N_DEFAULT_PHYSICS "N_BUILD_BULLET")
SET(N_PHYSICS ${N_DEFAULT_PHYSICS} CACHE STRING "Physics engine chosen by CMake")
SET_PROPERTY(CACHE N_PHYSICS PROPERTY STRINGS "N_BUILD_BULLET" "N_BUILD_HAVOK")
SET(${N_PHYSICS} ON)
												
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS  $<$<BOOL:${N_BUILD_HAVOK}>:__USE_HAVOK__> $<$<BOOL:${N_BUILD_BULLET}>:__USE_BULLET__>)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${N3ROOT}/libs/cmake)
INCLUDE(NebulaHelpers)

FIND_PACKAGE(FBX REQUIRED)
INCLUDE_DIRECTORIES(${FBX_INCLUDE_DIR})

INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/extlibs/raknet/source)


IF(WIN32)
		
        INCLUDE_DIRECTORIES($ENV{DXSDK_DIR}/include)
        IF(MSVC90)
                SET(MSVCFOLDER "vs9")
        ELSEIF(MSVC10)
                SET(MSVCFOLDER "vs10")
        ENDIF()

        if(CMAKE_CL_64)
                LINK_DIRECTORIES($ENV{DXSDK_DIR}/lib/x64)
        ELSE()
                LINK_DIRECTORIES($ENV{DXSDK_DIR}/lib/x86)
        ENDIF()
        SET(PROJECTBINDIR ${N3ROOT}/bin/win32)	
        ADD_DEFINITIONS(-DFBXSDK_NEW_API)	
ELSEIF(UNIX)
	
        ADD_DEFINITIONS(-DFBXSDK_NEW_API)

        LINK_DIRECTORIES(/usr/lib/;/usr/lib/x86_64-linux-gnu/)
        SET(PROJECTBINDIR ${N3ROOT}/bin/posix)		
		SET(NEBULA_TOOLKIT_LIBS ${FBX_LIBRARY} nvcore nvtt nvimage nvmath posh squish nvthread bc6h bc7)
       
ENDIF()

SET(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

#include(libs/CMakeLists.txt)
ADD_SUBDIRECTORY(code)
