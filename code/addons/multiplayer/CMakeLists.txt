#-------------------------------------------------------------------------------
# network
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code
					${Nebula3_SOURCE_DIR}/code/addons/
					${Nebula3_SOURCE_DIR}/code/addons/network/
					${Nebula3_SOURCE_DIR}/code/foundation/
					${Nebula3_SOURCE_DIR}/code/extlibs/
					${Nebula3_SOURCE_DIR}/code/extlibs/raknet/source
					${Nebula3_SOURCE_DIR}/code/render/	
					${Nebula3_SOURCE_DIR}/code/application
					${Nebula3_SOURCE_DIR}/code/application/basegamefeature					
					${CMAKE_CURRENT_SOURCE_DIR} 
					${CMAKE_BINARY_DIR}/code/extlibs/jansson/include
					${RAKNET_INCLUDE}										
					)

		FILE(GLOB SRC "*.cc")
		FILE(GLOB HEAD "*.h")
		LIST(APPEND SRCSBASE ${SRC} ${HEAD})
		SOURCE_GROUP("multiplayer" FILES ${SRCSBASE})
		
		FILE(GLOB SRC "properties/*.cc")
		FILE(GLOB HEAD "properties/*.h")
		LIST(APPEND SRCSPROP ${SRC} ${HEAD})
		SOURCE_GROUP("multiplayer\\properties" FILES ${SRCSPROP})

        
	set(SOURCES_files
		../stdneb.h
		../stdneb.cc
	)
	SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
	
	set(P_NIDL_FILES 		
		multiplayerattrs.nidl		
	)
	
	N_WRAP_NIDL_FILES(multiplayer_NIDLS ${P_NIDL_FILES})
	

add_library( multiplayer STATIC ${SRCSBASE} ${SRCSPROP} ${SOURCES_files})
SET_TARGET_PROPERTIES(multiplayer PROPERTIES FOLDER "N3SDK/addons")

ADD_DEPENDENCIES(multiplayer application RakNetLibStatic )


N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(multiplayer stdneb.h stdneb.cc ../)
ENDIF()
 
 
