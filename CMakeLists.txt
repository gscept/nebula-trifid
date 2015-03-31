set_property(GLOBAL PROPERTY USE_FOLDERS On)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

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
#IF(MSVC)
    OPTION(N_USE_PRECOMPILED_HEADERS "Use precompiled headers." ON)
#ENDIF()

SET(N_MATH_XNA OFF)
SET(N_MATH_VECMATH OFF)

IF(WIN32)
	SET(DEFMATH "N_MATH_XNA")
ELSE()
	SET(DEFMATH "N_MATH_VECMATH")
ENDIF()

SET(N_MATH ${DEFMATH} CACHE STRING "Math library used")
SET_PROPERTY(CACHE N_MATH PROPERTY STRINGS "N_MATH_XNA" "N_MATH_VECMATH")
SET(${N_MATH} ON)

IF(N_MATH_XNA)
	ADD_DEFINITIONS(-D__USE_XNA)
ELSE()
	ADD_DEFINITIONS(-D__USE_VECMATH)
	INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/extlibs/bullet/bullet/src)
ENDIF()


SET(N_BUILD_DX11 OFF)
SET(N_BUILD_OGL4 OFF)
SET(N_BUILD_DX9 OFF)

IF(WIN32)
	SET(DEFAULTRENDERER "N_BUILD_OGL4")
ELSEIF(UNIX)
	SET(DEFAULTRENDERER "N_BUILD_OGL4")
ENDIF()

SET(N_RENDERER ${DEFAULTRENDERER} CACHE STRING "Renderer chosen by CMake based on operating system")
SET_PROPERTY(CACHE N_RENDERER PROPERTY STRINGS "N_BUILD_DX11" "N_BUILD_OGL4" "N_BUILD_DX9")
SET(${N_RENDERER} ON)

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
OPTION(N_ENABLE_NETWORK "Build network related projects, currently broken" OFF)

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

SET(N_BUILD_BULLET OFF)
SET(N_BUILD_PHYSX OFF)
SET(N_BUILD_HAVOK OFF)
SET(N_DEFAULT_PHYSICS "N_BUILD_BULLET")
SET(N_PHYSICS ${N_DEFAULT_PHYSICS} CACHE STRING "Physics engine chosen by CMake")
SET_PROPERTY(CACHE N_PHYSICS PROPERTY STRINGS "N_BUILD_BULLET" "N_BUILD_PHYSX" "N_BUILD_HAVOK")
SET(${N_PHYSICS} ON)

SET(N_BUILD_LIBROCKET OFF)
SET(N_BUILD_CEUI OFF)
SET(N_DEFAULT_UI "N_BUILD_LIBROCKET")
SET(N_UI ${N_DEFAULT_UI} CACHE STRING "UI backend chosen by CMake")
SET_PROPERTY(CACHE N_UI PROPERTY STRINGS "N_BUILD_LIBROCKET" "N_BUILD_CEUI")
SET(${N_UI} ON)

#-------------------------------------------------------------------------------
# Sets up the default compiler options for a Nebula library target or executable.
#-------------------------------------------------------------------------------

MACRO(N_SET_COMPILER_DEFAULTS)

    IF(MSVC)
			SET(CMAKE_EXE_LINKER_FLAGS "/ignore:4099")
                IF(CMAKE_CL_64)
                # C++ compiler
                        SET(CMAKE_CXX_FLAGS 
                                "/D__WIN32__ /DWIN32 /DWIN64 /D__WIN64__ /D_HAS_EXCEPTIONS=0 /GF /fp:fast /W3 /WX /nologo /errorReport:prompt" 
                                CACHE STRING "Flags used by the compiler during all build types." FORCE)
                        SET(CMAKE_CXX_FLAGS_DEBUG
                                "/D_DEBUG /Od /Gm /RTC1 /MTd /Gy /Zi"
                                CACHE STRING "Flags used by the compiler during debug builds." FORCE)
                        SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO
                                "/Ob2 /Oi /Os /Oy /GL /FD /MT /GS- /Zi /MP"
                                CACHE STRING "Flags used by the compiler during release builds." FORCE)
                        SET(CMAKE_CXX_FLAGS_RELEASE
                                "/Ob2 /Oi /Os /Oy /GL /FD /MT /GS- /MP"
                                CACHE STRING "Flags used by the compiler during release builds." FORCE)
                        # C compiler (force compilation of .c files using C++ compiler)
                        SET(CMAKE_C_FLAGS 
                                "/D__WIN32__ /DWIN32 /DWIN64 /D__WIN64__ /D_HAS_EXCEPTIONS=0 /GF /fp:fast /W3 /WX /nologo /errorReport:prompt" 
                                CACHE STRING "Flags used by the compiler during all build types." FORCE)
                        SET(CMAKE_C_FLAGS_DEBUG
                                "/D_DEBUG /Od /Gm /RTC1 /MTd /Gy /Zi"
                                CACHE STRING "Flags used by the compiler during debug builds." FORCE)
                        SET(CMAKE_C_FLAGS_RELWITHDEBINFO
                                "/Ob2 /Oi /Os /Oy /GL /FD /MT /GS- /Zi /MP"
                                CACHE STRING "Flags used by the compiler during release builds." FORCE)
                        SET(CMAKE_C_FLAGS_RELEASE
                                "/Ob2 /Oi /Os /Oy /GL /FD /MT /GS- /MP"
                                CACHE STRING "Flags used by the compiler during release builds." FORCE)		
                ELSE()
                        # C++ compiler		
                        SET(CMAKE_CXX_FLAGS 
                                "/D__WIN32__ /DWIN32 /D_HAS_EXCEPTIONS=0 /GF /fp:fast /W3 /WX /MP /Oi /arch:SSE /arch:SSE2 /nologo /errorReport:prompt" 
                                CACHE STRING "Flags used by the compiler during all build types." FORCE)
                        SET(CMAKE_CXX_FLAGS_DEBUG
                                "/D_DEBUG /Od /RTC1 /MTd /Gy /ZI"
                                CACHE STRING "Flags used by the compiler during debug builds." FORCE)
                        SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO
                                "/Ob2 /Oi /Ot /Oy /GL /FD /MT /GS- /Zi"
                                CACHE STRING "Flags used by the compiler during release builds." FORCE)
                        SET(CMAKE_CXX_FLAGS_RELEASE
                                "/Ob2 /Oi /Ot /Oy /GL /FD /MT /GS- /DPUBLIC_BUILD"
                                CACHE STRING "Flags used by the compiler during release builds." FORCE)
                        # C compiler (force compilation of .c files using C++ compiler)
                        SET(CMAKE_C_FLAGS 
                                "/D__WIN32__ /DWIN32 /D_HAS_EXCEPTIONS=0 /GF /fp:fast /W3 /WX /MP /nologo /Oi /arch:SSE /arch:SSE2 /errorReport:prompt" 
                                CACHE STRING "Flags used by the compiler during all build types." FORCE)
                        SET(CMAKE_C_FLAGS_DEBUG
                                "/D_DEBUG /Od /RTC1 /MTd /Gy /ZI"
                                CACHE STRING "Flags used by the compiler during debug builds." FORCE)
                        SET(CMAKE_C_FLAGS_RELWITHDEBINFO
                                "/Ob2 /Oi /Ot /Oy /GL /FD /MT /GS- /Zi"
                                CACHE STRING "Flags used by the compiler during release builds." FORCE)
                        SET(CMAKE_C_FLAGS_RELEASE
                                "/Ob2 /Oi /Ot /Oy /GL /FD /MT /GS- /DPUBLIC_BUILD"
                                CACHE STRING "Flags used by the compiler during release builds." FORCE)
                ENDIF()
    ELSEIF(UNIX)
                # C compiler
                # remove multichar warning spewage
                SET(CMAKE_C_FLAGS "-Wno-multichar -Werror -msse4.2 -mavx2")
                SET(CMAKE_C_FLAGS_DEBUG "-g -D_DEBUG")
                SET(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -O3 -D_DEBUG")
                SET(CMAKE_C_FLAGS_RELEASE " -O3")
        
                # C++ compiler
                # remove multichar warning spewage
                SET(CMAKE_CXX_FLAGS "-Wno-multichar -fpermissive -std=gnu++0x -msse4.2 -mavx2")
                SET(CMAKE_CXX_FLAGS_DEBUG "-g -D_DEBUG")
                SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O3 -D_DEBUG")
                SET(CMAKE_CXX_FLAGS_RELEASE " -O3")
	ENDIF()
	
	ADD_DEFINITIONS(${N_DEFINITIONS})
ENDMACRO(N_SET_COMPILER_DEFAULTS)

# add all include and libs folders found in hierarchy

FILE(GLOB alldirs ./libs/*)
foreach(dir ${alldirs})
        if(IS_DIRECTORY ${dir})
                if(EXISTS ${dir}/CMakeLists.txt)
                        include(${dir}/CMakeLists.txt)
                else()
                    if(EXISTS ${dir}/include)
                            INCLUDE_DIRECTORIES(${dir}/include)
                    endif()
                    if(EXISTS ${dir}/libs)
                        if(WIN32)
                            if(EXISTS ${dir}/libs/win/x86)
                                LINK_DIRECTORIES(${dir}/libs/win/x86)
                                    if(MSVC90)
                                        LINK_DIRECTORIES(${dir}/libs/win/x86/vs9)
                                    elseif(MSVC10)
                                        LINK_DIRECTORIES(${dir}/libs/win/x86/vs10)
                                    endif()
                            endif()
                        elseif(UNIX)
                        endif()
                    endif()
                endif()            
        endif()
endforeach()

IF(N_BUILD_LIBROCKET)
	INCLUDE_DIRECTORIES(${N3ROOT}/code/extlibs/librocket)
ENDIF()

IF(N_BUILD_DX11)
        SET(PLATFORM_PROJECTS Effects11)
        INCLUDE_DIRECTORIES($ENV{DXSDK_DIR}/include)
        INCLUDE_DIRECTORIES(${N3ROOT}/code/extlibs/Effects11)
ELSEIF(N_BUILD_DX9)
        INCLUDE_DIRECTORIES($ENV{DXSDK_DIR}/include)
ELSEIF(N_BUILD_OGL4)
        INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/extlibs/glew/include)
        INCLUDE_DIRECTORIES(${N3ROOT}/code/extlibs/anyfx/api/code)
        INCLUDE_DIRECTORIES(${N3ROOT}/code/extlibs/anyfx/util/code)
        INCLUDE_DIRECTORIES(${N3ROOT}/code/extlibs/anyfx/compiler/code)
		INCLUDE_DIRECTORIES(${N3ROOT}/code/extlibs/anyfx/exts/eastl/include)
        INCLUDE_DIRECTORIES(${N3ROOT}/code/extlibs/glfw/include)
        SET(PLATFORM_PROJECTS compiler api glew EASTL SOIL)
        IF(WIN32)
            LIST(APPEND N_RENDER_LIBS glu32.lib opengl32.lib glfw)
        ELSE()
            LIST(APPEND N_RENDER_LIBS GL Xi Xrandr Xxf86vm glfw)
        ENDIF()
ENDIF()

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${N3ROOT}/libs/cmake)
FIND_PACKAGE(FBX REQUIRED)
INCLUDE_DIRECTORIES(${FBX_INCLUDE_DIR})
FIND_PACKAGE(FmodStudio REQUIRED)
INCLUDE_DIRECTORIES(${FMOD_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/extlibs/raknet/source)

IF(WIN32)
		
        INCLUDE_DIRECTORIES($ENV{DXSDK_DIR}/include)
        IF(MSVC90)
                SET(MSVCFOLDER "vs9")
        ELSEIF(MSVC10)
                SET(MSVCFOLDER "vs10")
        ENDIF()
		
        
				
        
        SET(N_EXT_LIBS ${FMOD_LIBRARIES})
		IF(N_BUILD_NVTT)
			SET(NEBULA_TOOLKIT_LIBS ${FBX_LIBRARY} nvcore nvtt nvimage nvmath posh squish nvthread bc6h bc7) 
		ELSE()
			SET(NEBULA_TOOLKIT_LIBS ${FBX_LIBRARY}) 
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
        SET(N_EXT_LIBS ${FMOD_LIBRARIES} rt)
		SET(NEBULA_TOOLKIT_LIBS ${FBX_LIBRARY} nvcore nvtt nvimage nvmath posh squish nvthread bc6h bc7)
ENDIF()

#include(libs/CMakeLists.txt)
ADD_SUBDIRECTORY(code)
