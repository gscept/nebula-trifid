#-------------------------------------------------------------------------------
# scripting
#-------------------------------------------------------------------------------

set(SOURCES_files_scripting__debug
	debug/scriptingpagehandler.cc
	debug/scriptingpagehandler.h
)
source_group("scripting\\debug" FILES ${SOURCES_files_scripting__debug})

set(SOURCES_scripting__debug
	${SOURCES_files_scripting__debug}
)

set(SOURCES_files_scripting__lua
	lua/luaserver.cc
	lua/luaserver.h
)
source_group("scripting\\lua" FILES ${SOURCES_files_scripting__lua})

set(SOURCES_scripting__lua
	${SOURCES_files_scripting__lua}
)

set(SOURCES_files_scripting
	arg.h
	argsblock.cc
	argsblock.h
	command.cc
	command.h
	scriptserver.cc
	scriptserver.h      
)
source_group("scripting" FILES ${SOURCES_files_scripting})

set(SOURCES_scripting      
	${SOURCES_scripting__debug}
	${SOURCES_scripting__lua}
	${SOURCES_files_scripting}
)
set(SOURCES_files
	../stdneb.h
	../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
set(P_NIDL_FILES 	commands/iolibrary.nidl 
					commands/stdlibrary.nidl
)



set(SOURCES_SRCS
	${SOURCES_scripting}
	${SOURCES_files}	
)
N_WRAP_ADD_NIDL_FILES(scripting SOURCES_SRCS ${P_NIDL_FILES})

add_library(scripting STATIC ${SOURCES_SRCS} )
TARGET_LINK_LIBRARIES(scripting lua foundation)
TARGET_INCLUDE_DIRECTORIES(scripting PUBLIC ${Nebula3_SOURCE_DIR}/code/extlibs ${Nebula3_SOURCE_DIR}/code/foundation ${Nebula3_SOURCE_DIR}/code/addons)
add_dependencies(scripting lua)
SET_TARGET_PROPERTIES(scripting PROPERTIES FOLDER "N3SDK/addons")

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(scripting stdneb.h stdneb.cc ../)
ENDIF()
 
