#-------------------------------------------------------------------------------
# ExtLibs
#-------------------------------------------------------------------------------


IF(NOT N_BUILD_FIRSTTIME)

	N_SET_COMPILER_DEFAULTS()

    IF(MSVC)
		ADD_SUBDIRECTORY(stackwalker)
		
		ADD_SUBDIRECTORY(libfreetype) 
    ENDIF()
	
	IF(N_BUILD_NVTT)
		ADD_SUBDIRECTORY(nvidia-texture-tools)
	ENDIF()

    ADD_SUBDIRECTORY(lua) 
	
    ADD_SUBDIRECTORY(recastnavigation/cmake)	
		SET_TARGET_PROPERTIES(DebugUtils PROPERTIES FOLDER "extlibs/recast")
		SET_TARGET_PROPERTIES(Detour PROPERTIES FOLDER "extlibs/recast")
		SET_TARGET_PROPERTIES(DetourCrowd PROPERTIES FOLDER "extlibs/recast")
		SET_TARGET_PROPERTIES(DetourTileCache PROPERTIES FOLDER "extlibs/recast")
		SET_TARGET_PROPERTIES(Recast PROPERTIES FOLDER "extlibs/recast")
		
    ADD_SUBDIRECTORY(bullet) 
    ADD_SUBDIRECTORY(tinyxml) 
    add_subdirectory(sqlite3) 
    ADD_SUBDIRECTORY(zlib) 
    ADD_SUBDIRECTORY(imgui)
    ADD_SUBDIRECTORY(libjpeg)
    ADD_SUBDIRECTORY(libpng) 	
    ADD_SUBDIRECTORY(CubeMapGen)
	ADD_SUBDIRECTORY(ogg)
		SET_TARGET_PROPERTIES(ogg PROPERTIES FOLDER "extlibs")
	ADD_SUBDIRECTORY(theora)
	
	#DevIL
	SET(LIBJPEG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libjpeg)
	#set(NVTT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nvidia-texture-tools)
	SET(LIBPNG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libpng)
	SET(LIBJPEG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libjpeg)
	ADD_SUBDIRECTORY(DevIL/DevIL)
		SET_TARGET_PROPERTIES(DevIL PROPERTIES FOLDER "extlibs/DevIL")
		SET_TARGET_PROPERTIES(ILU PROPERTIES FOLDER "extlibs/DevIL")
		SET_TARGET_PROPERTIES(ILUT PROPERTIES FOLDER "extlibs/DevIL")
		
			
	ADD_SUBDIRECTORY(raknet)
		SET_TARGET_PROPERTIES(RakNetLibStatic PROPERTIES FOLDER "extlibs")
	
	ADD_SUBDIRECTORY(miniupnp/miniupnpc)
		SET_TARGET_PROPERTIES(upnpc-static PROPERTIES FOLDER "extlibs")
		
	ADD_SUBDIRECTORY(base64)
	ADD_SUBDIRECTORY(cjson)
	ADD_SUBDIRECTORY(jzon-c)
	
	
    ADD_SUBDIRECTORY(librocket)    

    IF(N_BUILD_QT)
        ADD_SUBDIRECTORY(libqimg)		
    ENDIF()

    IF(N_BUILD_OGL4)			
        ADD_SUBDIRECTORY(glew)		
			SET_TARGET_PROPERTIES(glew PROPERTIES FOLDER "extlibs/glew")
        ADD_SUBDIRECTORY(SOIL)
		IF(MSVC)
				SET(BISON_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/anyfx/exts/glslang/tools/bison.exe")
			ENDIF()
		SET(ANYFX_FOLDER "extlibs/anyfx")
		SET(ANYFX_GLEW_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/glew)
			ADD_SUBDIRECTORY(anyfx)
			
			SET_TARGET_PROPERTIES(compiler PROPERTIES FOLDER "extlibs/anyfx")
			SET_TARGET_PROPERTIES(mcpp PROPERTIES FOLDER "extlibs/anyfx")
			SET_TARGET_PROPERTIES(antlr PROPERTIES FOLDER "extlibs/anyfx")
			SET_TARGET_PROPERTIES(api PROPERTIES FOLDER "extlibs/anyfx")
			SET_TARGET_PROPERTIES(util PROPERTIES FOLDER "extlibs/anyfx")
			SET_TARGET_PROPERTIES(EASTL PROPERTIES FOLDER "extlibs/anyfx")
		
		set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
		set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
		set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
		set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")		        
		ADD_SUBDIRECTORY(glfw)	
			SET_TARGET_PROPERTIES(glfw PROPERTIES FOLDER "extlibs/glfw")
    ELSEIF(N_BUILD_DX11)
        ADD_SUBDIRECTORY(Effects11)
	ELSEIF(N_BUILD_VULKAN)		
		SET(ANYFX_FOLDER "extlibs/anyfx")
			SET(ANYFX_VULKAN_FOLDER ${VULKAN_INCLUDE_DIR})
			SET(ANYFX_FOLDER "extlibs/anyfx")
			ADD_SUBDIRECTORY(anyfx)
			
			#add_subdirectory(SOIL) #still need the stb image stuff in SOIL
			
			#SET_TARGET_PROPERTIES(compiler PROPERTIES FOLDER "extlibs/anyfx")
			#SET_TARGET_PROPERTIES(mcpp PROPERTIES FOLDER "extlibs/anyfx")
			#SET_TARGET_PROPERTIES(antlr PROPERTIES FOLDER "extlibs/anyfx")
			#SET_TARGET_PROPERTIES(api PROPERTIES FOLDER "extlibs/anyfx")
			#SET_TARGET_PROPERTIES(util PROPERTIES FOLDER "extlibs/anyfx")
			#SET_TARGET_PROPERTIES(EASTL PROPERTIES FOLDER "extlibs/anyfx")
		
		set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
		set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
		set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
		set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")		        
		ADD_SUBDIRECTORY(glfw)	
			SET_TARGET_PROPERTIES(glfw PROPERTIES FOLDER "extlibs/glfw")
    ENDIF()

ELSE()
    add_subdirectory(tinyxml) 
    add_subdirectory(zlib)
    add_subdirectory(base64)
ENDIF()
