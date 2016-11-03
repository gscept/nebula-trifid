set_property(GLOBAL PROPERTY USE_FOLDERS On)
CMAKE_MINIMUM_REQUIRED(VERSION 3.4.0)

PROJECT(NebulaTrifid)

SET(N3ROOT ${Nebula3_SOURCE_DIR})

# for makefile based builds default to release builds
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

#-------------------------------------------------------------------------------
# Settings that can be tweaked by the user prior to building.
#-------------------------------------------------------------------------------

OPTION(N_USE_PRECOMPILED_HEADERS "Use precompiled headers." ON)

IF(MSVC)
OPTION(N_STATIC_BUILD "Use static runtime in windows build" ON)
ENDIF()

IF(N_STATIC_BUILD)
	ADD_DEFINITIONS(-D__N_STATIC_BUILD)
ENDIF()
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
	OPTION(N_USE_AVX "Use AVX instructionset" OFF)		
ENDIF()

OPTION(N_BUILD_OLD "Build old mostly obsolete things (some probably broken)." OFF)
OPTION(N_BUILD_PUBLIC_AS_RELEASE "Instead of outputting release build as name.public.exe they are called name.exe." OFF)
OPTION(N_BUILD_EXTERNAL_PROJECTS "Include projects included in the projects folder" ON)
OPTION(N_VULKAN_EXPERIMENTAL "Build NebulaT using an experimental (non-release) version of Vulkan" OFF)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${N3ROOT}/libs/cmake)
INCLUDE(NebulaHelpers)

# select render api
SET(N_BUILD_OGL4 OFF)
SET(N_BUILD_VULKAN OFF)

SET(DEFAULTRENDERER "N_BUILD_VULKAN")

SET(N_RENDERER ${DEFAULTRENDERER} CACHE STRING "Renderer chosen by CMake based on operating system")
SET_PROPERTY(CACHE N_RENDERER PROPERTY STRINGS "N_BUILD_OGL4" "N_BUILD_VULKAN")
SET(${N_RENDERER} ON)
# these are required for all extlibs packages that use precompiled headers from nebula
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS  
	$<$<BOOL:${N_BUILD_OGL4}>:__OGL4__ GLEW_STATIC __ANYFX__ IL_STATIC_LIB> 
	$<$<BOOL:${N_BUILD_VULKAN}>:__VULKAN__ __ANYFX__ IL_STATIC_LIB> 	
	)
	
IF(${N_BUILD_VULKAN})
	IF (${N_VULKAN_EXPERIMENTAL})
		FIND_PACKAGE(VulkanExperimental)
		MESSAGE("Using experimental Vulkan")
	ELSE()
		FIND_PACKAGE(Vulkan)
	ENDIF()
ENDIF()

INCLUDE_DIRECTORIES($<$<BOOL:${N_BUILD_OGL4}>:${Nebula3_SOURCE_DIR}/code/extlibs/anyfx/api/code
												${Nebula3_SOURCE_DIR}/code/extlibs/glew/include
												${Nebula3_SOURCE_DIR}/code/extlibs/anyfx/api/code
												${Nebula3_SOURCE_DIR}/code/extlibs/anyfx/util/code
												${Nebula3_SOURCE_DIR}/code/extlibs/anyfx/compiler/code
												${Nebula3_SOURCE_DIR}/code/extlibs/anyfx/exts/eastl/include
												${Nebula3_SOURCE_DIR}/code/extlibs/glfw/include>
					$<$<BOOL:${N_BUILD_VULKAN}>:${Nebula3_SOURCE_DIR}/code/extlibs/anyfx/api/lowlevel
												${Nebula3_SOURCE_DIR}/code/extlibs/anyfx/api
												${Nebula3_SOURCE_DIR}/code/extlibs/glew/include
												${VULKAN_INCLUDE_DIR}
												${Nebula3_SOURCE_DIR}/code/extlibs/anyfx/util/code
												${Nebula3_SOURCE_DIR}/code/extlibs/anyfx/compiler/code
												${Nebula3_SOURCE_DIR}/code/extlibs/anyfx/exts/eastl/include
												${Nebula3_SOURCE_DIR}/code/extlibs/glfw/include>
					)


# select physics implementation
SET(N_BUILD_BULLET OFF)
SET(N_BUILD_PHYSX OFF)
SET(N_DEFAULT_PHYSICS "N_BUILD_BULLET")
SET(N_PHYSICS ${N_DEFAULT_PHYSICS} CACHE STRING "Physics engine chosen by CMake")
SET_PROPERTY(CACHE N_PHYSICS PROPERTY STRINGS "N_BUILD_BULLET" "N_BUILD_PHYSX")
SET(${N_PHYSICS} ON)
												
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS  $<$<BOOL:${N_BUILD_BULLET}>:__USE_BULLET__> $<$<BOOL:${N_BUILD_PHYSX}>:__USE_PHYSX__>)

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
ELSEIF(UNIX)
	OPTION(N_BUILD_OGL4 "Build using OpenGL 4 renderer" ON)
	
	FIND_LIBRARY(UUID NAMES libuuid.so) #uuid lib
	FIND_LIBRARY(LIBC NAMES libc.so) #libc
	FIND_LIBRARY(LIBDL NAMES libdl.so) #lib for dynamic linkage
	FIND_PACKAGE(Freetype )
	FIND_PACKAGE(JPEG)
	FIND_PACKAGE(PNG)
	FIND_PACKAGE(Boost REQUIRED)
	LINK_DIRECTORIES(/usr/lib/;/usr/lib/x86_64-linux-gnu/)
	SET(PROJECTBINDIR ${N3ROOT}/bin/posix)	
	IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux specific code
		SET(LINUX true)
	ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")		
ENDIF()

# help find the nidl files that live in the binary directory now
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/nidl/)

SET(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

ADD_SUBDIRECTORY(code)
