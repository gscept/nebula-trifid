

# help find the nidl files that live in the binary directory now
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/nidl/)


IF(WIN32)
	IF(N_MATH_XNA)
		SET(N_BASE_LIBS dxguid.lib;d3d9.lib;d3dx9.lib;dinput8.lib;xinput.lib;dxerr.lib;x3daudio.lib;d3dx11.lib;dxgi.lib;d2d1.lib;dwrite.lib;d3dcompiler.lib;d3d11.lib;dbghelp.lib;wsock32.lib;rpcrt4.lib;wininet.lib;DbgHelp.lib)
	ELSE()
		SET(N_BASE_LIBS dbghelp.lib;wsock32.lib;rpcrt4.lib;wininet.lib;DbgHelp.lib)
	ENDIF()

    SET(ROCKET libfreetype librocket)

    SET(DEVIL DevIL ILU ILUT)

    SET(N_LIBS ${N_BASE_LIBS} ${N_EXT_LIBS})

    if(CMAKE_CL_64)
        SET(N_LIBDIRS ${N_LIBDIRS} $ENV{DXSDK_DIR}/lib/x64)
    ELSE()
        SET(N_LIBDIRS ${N_LIBDIRS} $ENV{DXSDK_DIR}/lib/x86)
    ENDIF()

    SET(PROJECTBINDIR ${N3ROOT}/bin/win32)

ELSEIF(UNIX)
	OPTION(N_BUILD_OGL4 "Build using OpenGL 4 renderer" ON)

	#FIND_LIBRARY(DEVIL NAMES libIL.so libILU.so)
	FIND_LIBRARY(UUID NAMES libuuid.so) #uuid lib
	FIND_LIBRARY(LIBC NAMES libc.so) #libc
	FIND_LIBRARY(LIBDL NAMES libdl.so) #lib for dynamic linkage
	FIND_PACKAGE(Freetype )
	FIND_PACKAGE(JPEG)
	FIND_PACKAGE(PNG)
	FIND_PACKAGE(Boost REQUIRED)
	
	SET(N_EXTRA_LIBS "pthread ${UUID} ${LIBC} ${LIBDL}")
	SET(N_LIBDIRS /usr/lib/;/usr/lib/x86_64-linux-gnu/)
	SET(PROJECTBINDIR ${N3ROOT}/bin/posix)	
	
	SET(N_LIBS ${N_EXTRA_LIBS})
	IF(N_BUILD_FIRSTTIME)
		SET(N_LIBS ${N_EXTRA_LIBS})
	ELSE()
	        SET(N_LIBS ${N_EXTRA_LIBS} ${N_EXT_LIBS} ${FREETYPE_LIBRARIES})
	ENDIF()
	
	IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux specific code
		SET(LINUX true)
	ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
ENDIF()

IF(N_BUILD_DX11)
		SET(N_DEFINITIONS ${N_DEFINITIONS} -D__DX11__)
ELSEIF(N_BUILD_DX9)
		SET(N_DEFINITIONS ${N_DEFINITIONS} -D__DX9__)
ELSEIF(N_BUILD_OGL4)
#		SET(N_DEFINITIONS ${N_DEFINITIONS} -D__OGL4__ -DGLEW_STATIC -D__ANYFX__  -DIL_STATIC_LIB)
ENDIF()


#-------------------------------------------------------------------------------
# convenience variable for changing project dependencies on a global scale 
# should be split into sections
#-------------------------------------------------------------------------------

SET(NEBULA_DEFAULT_LIBS
				navigation navigation_NIDLS Detour DetourCrowd Recast DetourTileCache DebugUtils
				${DEVIL}
                application application_NIDLS
                faudio faudio_NIDLS                 
                framecapture framecapture_NIDLS
                posteffect
                render                                                
                remote remote_NIDLS 
                posteffect_NIDLS render_visibilityboxsystemjob render_visibilityquadtreejob
				imgui dynui
                render_animjobsamplemix render_animjobsample render_particlejob render_charevalskeletonjob ${PHYSICS_DEPENDENCIES}
                render_NIDLS ui scripting scripting_NIDLS lua effects effects_NIDLS forest forest_NIDLS
                vibration vibration_NIDLS                   
                foundation
                ui_NIDLS
				multiplayer multiplayer_NIDLS RakNetLibStatic upnpc-static jansson 
                ${PLATFORM_PROJECTS} ${N_RENDER_LIBS}
                ${N_LIBS}
                ${ROCKET}
)


IF(N_BUILD_QT AND NOT N_BUILD_FIRSTTIME)
	FIND_PACKAGE(Qt4 REQUIRED QtCore QtGui QtXml QtNetwork)

	IF(N_QT_NEB_SUPPORT)
		ADD_DEFINITIONS(-D_QT_NEB_SUPPORT)
		INCLUDE(${QT_USE_FILE})
		ADD_DEFINITIONS(${QT_DEFINITIONS})
		LIST(APPEND NEBULA_DEFAULT_LIBS qtfeature 	${QT_LIBS} ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} )
	ENDIF()
ENDIF()


# upon bootstrap build, only build foundation and tools

ADD_SUBDIRECTORY(foundation)
ADD_SUBDIRECTORY(extlibs)

IF(N_BUILD_FIRSTTIME)
	ADD_SUBDIRECTORY(toolkit)
ELSE()
	ADD_SUBDIRECTORY(render)    
    ADD_SUBDIRECTORY(addons)
    ADD_SUBDIRECTORY(application)
        

	IF(N_BUILD_BENCHMARKS)
		ADD_SUBDIRECTORY(benchmarks)
	ENDIF()
	IF(N_BUILD_TESTS)
		ADD_SUBDIRECTORY(tests)
	ENDIF()
	IF(N_BUILD_TOOLKIT)
		ADD_SUBDIRECTORY(toolkit)
		IF(N_BUILD_QT)
			ADD_SUBDIRECTORY(qttoolkit)
		ENDIF()
	ENDIF()
	if(N_BUILD_PLUGIN)
		IF(CMAKE_CL_64)
			ADD_SUBDIRECTORY(mayaplugin)
		ENDIF()
	ENDIF()

	IF(N_BUILD_EXTERNAL_PROJECTS)
		ADD_SUBDIRECTORY(../projects ${CMAKE_BINARY_DIR}/projects)
	ENDIF()
ENDIF()

