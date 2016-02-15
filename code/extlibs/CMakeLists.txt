#-------------------------------------------------------------------------------
# ExtLibs
#-------------------------------------------------------------------------------


IF(NOT N_BUILD_FIRSTTIME)

	N_SET_COMPILER_DEFAULTS()

    IF(MSVC)
		add_subdirectory(stackwalker)
		
		add_subdirectory(libfreetype) 
    ENDIF()
	
	IF(N_BUILD_NVTT)
		add_subdirectory(nvidia-texture-tools)
	ENDIF()

    add_subdirectory(lua) 
	
    add_subdirectory(recastnavigation/cmake)	
		SET_TARGET_PROPERTIES(DebugUtils PROPERTIES FOLDER "extlibs/recast")
		SET_TARGET_PROPERTIES(Detour PROPERTIES FOLDER "extlibs/recast")
		SET_TARGET_PROPERTIES(DetourCrowd PROPERTIES FOLDER "extlibs/recast")
		SET_TARGET_PROPERTIES(DetourTileCache PROPERTIES FOLDER "extlibs/recast")
		SET_TARGET_PROPERTIES(Recast PROPERTIES FOLDER "extlibs/recast")
		
    add_subdirectory(bullet) 
    add_subdirectory(tinyxml) 
    add_subdirectory(sqlite3) 
    add_subdirectory(zlib) 
    add_subdirectory(imgui)
    add_subdirectory(libjpeg)
    add_subdirectory(libpng) 	
    add_subdirectory(CubeMapGen)
	add_subdirectory(ogg)
		SET_TARGET_PROPERTIES(ogg PROPERTIES FOLDER "extlibs")
	add_subdirectory(theora)
	
	#DevIL
	set(LIBJPEG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libjpeg)
	#set(NVTT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nvidia-texture-tools)
	set(LIBPNG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libpng)
	set(LIBJPEG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libjpeg)
	add_subdirectory(DevIL/DevIL)
		SET_TARGET_PROPERTIES(DevIL PROPERTIES FOLDER "extlibs/DevIL")
		SET_TARGET_PROPERTIES(ILU PROPERTIES FOLDER "extlibs/DevIL")
		SET_TARGET_PROPERTIES(ILUT PROPERTIES FOLDER "extlibs/DevIL")
		
			
	add_subdirectory(raknet)
		SET_TARGET_PROPERTIES(RakNetLibStatic PROPERTIES FOLDER "extlibs")
	
	add_subdirectory(miniupnp/miniupnpc)
		SET_TARGET_PROPERTIES(upnpc-static PROPERTIES FOLDER "extlibs")
	
	set(JANSSON_STATIC_CRT ON CACHE BOOL "Link the static CRT libraries")
	set(JANSSON_EXAMPLES OFF CACHE BOOL "Compile example applications")
	set(JANSSON_BUILD_DOCS OFF CACHE BOOL "Build documentation (uses python-sphinx).")
	set(JANSSON_WITHOUT_TESTS ON CACHE BOOL "Don't build tests ('make test' to execute tests)")
	add_subdirectory(jansson)
		SET_TARGET_PROPERTIES(jansson PROPERTIES FOLDER "extlibs")
	add_subdirectory(base64)
	
	
    add_subdirectory(librocket)    

    IF(N_BUILD_QT)
        add_subdirectory(libqimg)
    ENDIF()

    IF(N_BUILD_OGL4)			
        add_subdirectory(glew)		
			SET_TARGET_PROPERTIES(glew PROPERTIES FOLDER "extlibs/glew")
        add_subdirectory(SOIL)
		IF(MSVC)
				SET(BISON_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/anyfx/exts/glslang/tools/bison.exe")
			ENDIF()
		SET(ANYFX_FOLDER "extlibs/anyfx")
        add_subdirectory(anyfx)
			SET(ANYFX_GLEW_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/glew)
			SET_TARGET_PROPERTIES(compiler PROPERTIES FOLDER "extlibs/anyfx")
			SET_TARGET_PROPERTIES(mcpp PROPERTIES FOLDER "extlibs/anyfx")
			SET_TARGET_PROPERTIES(antlr PROPERTIES FOLDER "extlibs/anyfx")
			SET_TARGET_PROPERTIES(api PROPERTIES FOLDER "extlibs/anyfx")
			SET_TARGET_PROPERTIES(util PROPERTIES FOLDER "extlibs/anyfx")
			SET_TARGET_PROPERTIES(EASTL PROPERTIES FOLDER "extlibs/anyfx/exts")
			SET_TARGET_PROPERTIES(glslang PROPERTIES FOLDER "extlibs/anyfx/exts")
			SET_TARGET_PROPERTIES(glslangValidator PROPERTIES FOLDER "extlibs/anyfx/exts")
			SET_TARGET_PROPERTIES(OGLCompiler PROPERTIES FOLDER "extlibs/anyfx/exts")
			SET_TARGET_PROPERTIES(OSDependent PROPERTIES FOLDER "extlibs/anyfx/exts")
			SET_TARGET_PROPERTIES(SPIRV PROPERTIES FOLDER "extlibs/anyfx/exts")
			SET_TARGET_PROPERTIES(spirv-remap PROPERTIES FOLDER "extlibs/anyfx/exts")
			
		
		set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
		set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
		set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
		set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")		        
		add_subdirectory(glfw)	
			SET_TARGET_PROPERTIES(glfw PROPERTIES FOLDER "extlibs/glfw")
    ELSEIF(N_BUILD_DX11)
        add_subdirectory(Effects11)
    ENDIF()

ELSE()
    add_subdirectory(tinyxml) 
    add_subdirectory(zlib)
    add_subdirectory(base64)
ENDIF()
