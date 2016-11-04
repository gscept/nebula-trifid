# nebula macros for cmake

#------------------------------------------------------------------------------
# macro for nidl files
#------------------------------------------------------------------------------

MACRO(N_WRAP_NIDL_FILES proj)		
        set(files ${ARGN})
		set(outlist "")
        List(APPEND outlist ${argn})
        SOURCE_GROUP("NIDL Files" FILES ${files})
        FOREACH(nidl ${files})
                STRING(REPLACE ".nidl" ".cc" outfile ${nidl}) 
                STRING(REPLACE ".nidl" ".h" outfileh ${nidl})
                STRING(FIND "${CMAKE_CURRENT_SOURCE_DIR}"  "/" last REVERSE)
                STRING(SUBSTRING "${CMAKE_CURRENT_SOURCE_DIR}" ${last}+1 -1 folder)
                IF(WIN32)
                    ADD_CUSTOM_COMMAND( OUTPUT "${CMAKE_BINARY_DIR}/nidl/${outfile}" "${CMAKE_BINARY_DIR}/nidl/${outfileh}"
                                        PRE_BUILD COMMAND "${N3ROOT}/bin/win32/idlc.toolkit.exe" -output "${CMAKE_BINARY_DIR}/nidl/" "${nidl}"
                                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" 
                                        MAIN_DEPENDENCY "${nidl}"
                                        VERBATIM PRE_BUILD)
                ELSEIF(UNIX)
                    ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_BINARY_DIR}/nidl/${outfile} ${CMAKE_BINARY_DIR}/nidl/${outfileh}
                                        COMMAND ${N3ROOT}/bin/posix/idlc.toolkit -output ${CMAKE_BINARY_DIR}/nidl/ ${nidl}
                                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
                                        MAIN_DEPENDENCY ${nidl}
                                        )
                ENDIF()				
                SOURCE_GROUP("Generated" FILES "${CMAKE_BINARY_DIR}/nidl/${outfile}" "${CMAKE_BINARY_DIR}/nidl/${outfileh}")
                List(APPEND outlist ${nidl} "${CMAKE_BINARY_DIR}/nidl/${outfile}" "${CMAKE_BINARY_DIR}/nidl/${outfileh}")
        ENDFOREACH()
        ADD_LIBRARY(${proj} STATIC ${outlist})
        SET_TARGET_PROPERTIES(${proj} PROPERTIES FOLDER "NIDL")
ENDMACRO()

MACRO(N_WRAP_ADD_NIDL_FILES projname proj)		
        set(files ${ARGN})
		set(outlist "")
        List(APPEND outlist ${ARGN})		
        SOURCE_GROUP("NIDL Files" FILES ${files})
        FOREACH(nidl ${files})
                STRING(REPLACE ".nidl" ".cc" outfile ${nidl}) 
                STRING(REPLACE ".nidl" ".h" outfileh ${nidl})
                STRING(FIND "${CMAKE_CURRENT_SOURCE_DIR}"  "/" last REVERSE)
                STRING(SUBSTRING "${CMAKE_CURRENT_SOURCE_DIR}" ${last}+1 -1 folder)				
                IF(WIN32)
                    ADD_CUSTOM_COMMAND( OUTPUT "${CMAKE_BINARY_DIR}/nidl/${projname}/${outfile}" "${CMAKE_BINARY_DIR}/nidl/${projname}/${outfileh}"
                                        PRE_BUILD COMMAND "${N3ROOT}/bin/win32/idlc.toolkit.exe" -output "${CMAKE_BINARY_DIR}/nidl/${projname}/" "${nidl}"
                                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" 
                                        MAIN_DEPENDENCY "${nidl}"
                                        VERBATIM PRE_BUILD)
                ELSEIF(UNIX)
                    ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_BINARY_DIR}/nidl/${projname}/${outfile} ${CMAKE_BINARY_DIR}/nidl/${projname}/${outfileh}
                                        COMMAND ${N3ROOT}/bin/posix/idlc.toolkit -output ${CMAKE_BINARY_DIR}/nidl/${projname}/ ${nidl}
                                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
                                        MAIN_DEPENDENCY ${nidl}
                                        )
                ENDIF()				
                SOURCE_GROUP("Generated" FILES "${CMAKE_BINARY_DIR}/nidl/${projname}/${outfile}" "${CMAKE_BINARY_DIR}/nidl/${projname}/${outfileh}")
                List(APPEND outlist ${nidl} "${CMAKE_BINARY_DIR}/nidl/${projname}/${outfile}" "${CMAKE_BINARY_DIR}/nidl/${projname}/${outfileh}")
        ENDFOREACH()
		List(APPEND ${proj} ${outlist})
ENDMACRO()


MACRO(N_WRAP_ADD_NIDL_FILES proj)		
        set(files ${ARGN})
		set(outlist "")
        List(APPEND outlist ${ARGN})		
        SOURCE_GROUP("NIDL Files" FILES ${files})
        FOREACH(nidl ${files})
                STRING(REPLACE ".nidl" ".cc" outfile ${nidl}) 
                STRING(REPLACE ".nidl" ".h" outfileh ${nidl})
                STRING(FIND "${CMAKE_CURRENT_SOURCE_DIR}"  "/" last REVERSE)
                STRING(SUBSTRING "${CMAKE_CURRENT_SOURCE_DIR}" ${last}+1 -1 folder)				
                IF(WIN32)
                    ADD_CUSTOM_COMMAND( OUTPUT "${CMAKE_BINARY_DIR}/nidl/${outfile}" "${CMAKE_BINARY_DIR}/nidl/${outfileh}"
                                        PRE_BUILD COMMAND "${N3ROOT}/bin/win32/idlc.toolkit.exe" -output "${CMAKE_BINARY_DIR}/nidl/" "${nidl}"
                                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" 
                                        MAIN_DEPENDENCY "${nidl}"
                                        VERBATIM PRE_BUILD)
                ELSEIF(UNIX)
                    ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_BINARY_DIR}/nidl/${outfile} ${CMAKE_BINARY_DIR}/nidl/${outfileh}
                                        COMMAND ${N3ROOT}/bin/posix/idlc.toolkit -output ${CMAKE_BINARY_DIR}/nidl/ ${nidl}
                                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
                                        MAIN_DEPENDENCY ${nidl}
                                        )
                ENDIF()				
                SOURCE_GROUP("Generated" FILES "${CMAKE_BINARY_DIR}/nidl/${outfile}" "${CMAKE_BINARY_DIR}/nidl/${outfileh}")
                List(APPEND outlist ${nidl} "${CMAKE_BINARY_DIR}/nidl/${outfile}" "${CMAKE_BINARY_DIR}/nidl/${outfileh}")
        ENDFOREACH()
		List(APPEND ${proj} ${outlist})
ENDMACRO()

MACRO(N_WRAP_ADD_PROJ_NIDL_FILES projname proj)		
        set(files ${ARGN})
		set(outlist "")
        List(APPEND outlist ${ARGN})		
        SOURCE_GROUP("NIDL Files" FILES ${files})
        FOREACH(nidl ${files})
                STRING(REPLACE ".nidl" ".cc" outfile ${nidl}) 
                STRING(REPLACE ".nidl" ".h" outfileh ${nidl})
                STRING(FIND "${CMAKE_CURRENT_SOURCE_DIR}"  "/" last REVERSE)
                STRING(SUBSTRING "${CMAKE_CURRENT_SOURCE_DIR}" ${last}+1 -1 folder)				
                IF(WIN32)
                    ADD_CUSTOM_COMMAND( OUTPUT "${CMAKE_BINARY_DIR}/nidl/${projname}/${outfile}" "${CMAKE_BINARY_DIR}/nidl/${projname}/${outfileh}"
                                        PRE_BUILD COMMAND "${N3ROOT}/bin/win32/idlc.toolkit.exe" -output "${CMAKE_BINARY_DIR}/nidl/${projname}/" "${nidl}"
                                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" 
                                        MAIN_DEPENDENCY "${nidl}"
                                        VERBATIM PRE_BUILD)
                ELSEIF(UNIX)
                    ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_BINARY_DIR}/nidl/${projname}/${outfile} ${CMAKE_BINARY_DIR}/nidl/${projname}/${outfileh}
                                        COMMAND ${N3ROOT}/bin/posix/idlc.toolkit -output ${CMAKE_BINARY_DIR}/nidl/${projname}/ ${nidl}
                                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
                                        MAIN_DEPENDENCY ${nidl}
                                        )
                ENDIF()				
                SOURCE_GROUP("Generated" FILES "${CMAKE_BINARY_DIR}/nidl/${projname}/${outfile}" "${CMAKE_BINARY_DIR}/nidl/${projname}/${outfileh}")
                List(APPEND outlist ${nidl} "${CMAKE_BINARY_DIR}/nidl/${projname}/${outfile}" "${CMAKE_BINARY_DIR}/nidl/${projname}/${outfileh}")
        ENDFOREACH()
		List(APPEND ${proj} ${outlist})
ENDMACRO()

#-------------------------------------------------------------------------------
# helper for adding xml template files, blueprints and gui files to the project
#-------------------------------------------------------------------------------

MACRO(N_ADD_PROJECT_FILES files)
	# template and other xml files
	FILE(GLOB XMLS "../../data/tables/db/*.xml")
	SOURCE_GROUP("templates" FILES ${XMLS})
	LIST(APPEND XMLS "../../data/tables/blueprints.xml" "../../data/tables/globals.xml")
	LIST(APPEND ${files} ${XMLS})
	# scripts
	FILE(GLOB_RECURSE LUA "../../data/scripts/*.lua")
	SOURCE_GROUP("scripts" FILES ${LUA})
	LIST(APPEND ${files} ${LUA})
	# gui
	FILE(GLOB_RECURSE UI "../../data/gui/*.rml")
	SOURCE_GROUP("ui\\rml" FILES ${UI})
	LIST(APPEND ${files} ${UI})
	FILE(GLOB_RECURSE UI "../../data/gui/*.lua")
	SOURCE_GROUP("ui\\scripts" FILES ${UI})
	LIST(APPEND ${files} ${UI})
ENDMACRO()

#-------------------------------------------------------------------------------
# Sets up the default Nebula precompiled header for the given target.
# param TargetName is the name of a target that should use precompiled headers.
# See extlibs/tinyxml/CMakeLists.txt for an example.
#-------------------------------------------------------------------------------
MACRO(N_SET_PCH TargetName HeaderName SourceName)
    SET(SourceFilePath "")
    IF(${ARGC} STREQUAL 4)
        SET(SourceFilePath ${ARGV3})
    ENDIF()
    IF(MSVC)
        SET_TARGET_PROPERTIES(${TargetName}
            PROPERTIES
            COMPILE_FLAGS "/Yu${HeaderName} /Fp${CMAKE_CFG_INTDIR}/${TargetName}.pch")
        SET_SOURCE_FILES_PROPERTIES(${SourceFilePath}${SourceName}
            PROPERTIES
            COMPILE_FLAGS "/Yc${HeaderName}")
    ELSE()        
        if(${ARGC} STREQUAL 4)
            SET(HEADER "${ARGV3}${HeaderName}")
        else()
            SET(HEADER ${HeaderName})
        endif()
        GET_FILENAME_COMPONENT(_name ${HEADER} NAME)
                SET(_source "${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}")
        SET(_outdir "${CMAKE_CURRENT_BINARY_DIR}/${_name}.gch")
        MAKE_DIRECTORY(${_outdir})
        SET(_output "${_outdir}/.c++")
        
        STRING(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name)
        SET(_compiler_FLAGS ${${_flags_var_name}})

        STRING(TOUPPER "CMAKE_CXX_FLAGS" _flags_var_name2)
        LIST(APPEND _compiler_FLAGS ${${_flags_var_name2}})
        
        GET_DIRECTORY_PROPERTY(_directory_flags INCLUDE_DIRECTORIES)
        FOREACH(item ${_directory_flags})
        LIST(APPEND _compiler_FLAGS "-I${item}")
        ENDFOREACH(item)
    
        GET_DIRECTORY_PROPERTY(_directory_flags DEFINITIONS)
        LIST(APPEND _compiler_FLAGS ${_directory_flags})
    
        SEPARATE_ARGUMENTS(_compiler_FLAGS)
        #MESSAGE("${CMAKE_CXX_COMPILER} -DPCHCOMPILE ${_compiler_FLAGS} -x c++-header -o {_output} ${_source}")
        ADD_CUSTOM_COMMAND(
        OUTPUT ${_output}
        COMMAND ${CMAKE_CXX_COMPILER} ${_compiler_FLAGS} -x c++-header -o ${_output} ${_source}
        DEPENDS ${_source} )
        ADD_CUSTOM_TARGET(${TargetName}_gch DEPENDS ${_output})
        ADD_DEPENDENCIES(${TargetName} ${TargetName}_gch)
        SET_TARGET_PROPERTIES(${TargetName} PROPERTIES COMPILE_FLAGS "-include ${_name} -Winvalid-pch")
    ENDIF()
ENDMACRO(N_SET_PCH)

#-------------------------------------------------------------------------------
# Sets the subsystem linker option on WIN32, when the subsystem is set to
# "Windows" the executable is expected to have a WinMain(), if "Console" is
# defined the executable is expected to have a main().
#-------------------------------------------------------------------------------
MACRO(N_SET_EXE_SUBSYSTEM TargetName SubsystemName)
    IF(WIN32)
        IF(${SubsystemName} STREQUAL Windows)
            SET_TARGET_PROPERTIES(${TargetName}
                PROPERTIES
                WIN32_EXECUTABLE ON
            )
        ELSEIF(${SubsystemName} STREQUAL Console)
            SET_TARGET_PROPERTIES(${TargetName}
                PROPERTIES
                WIN32_EXECUTABLE OFF
            )
        ENDIF()
    ENDIF()
ENDMACRO(N_SET_EXE_SUBSYSTEM)

#-------------------------------------------------------------------------------
# Option for enabling link time code generation in windows
#-------------------------------------------------------------------------------

IF(MSVC)
OPTION(N_ENABLE_LTCG "Enable link time code generation in visual studio (large compile time increase)" OFF)
IF(N_ENABLE_LTCG)
	SET(N_LTCG "/LTCG")
	SET(N_GL "/GL")
ENDIF()
ENDIF()

#-------------------------------------------------------------------------------
# Sets up the default linker options for a Nebula executable.
#-------------------------------------------------------------------------------
MACRO(N_SET_EXE_LINKER_DEFAULTS TargetName)
    IF(MSVC)
        SET(CMAKE_EXE_LINKER_FLAGS_DEBUG
            "/MANIFEST:NO /INCREMENTAL:YES /DEBUG"
            CACHE STRING "Flags used by the linker when building an executable." FORCE
        )
		
		SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
            "/MANIFEST:NO /INCREMENTAL:NO ${N_LTCG} /DEBUG"
            CACHE STRING "Flags used by the linker when building an executable." FORCE
        )
		
		SET(CMAKE_EXE_LINKER_FLAGS_RELEASE
            "/MANIFEST:NO /INCREMENTAL:NO ${N_LTCG}"
            CACHE STRING "Flags used by the linker when building an executable." FORCE
        )
		
    ENDIF()
    
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECTBINDIR})
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECTBINDIR})
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECTBINDIR})
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECTBINDIR})
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECTBINDIR})
    
    IF(MSVC)
		SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEBUG_POSTFIX ".debug")
    ENDIF()
    
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES PDB_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/pdb)
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pdb)	
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/pdb)
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES PDB_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/pdb)
    
    IF(NOT N_BUILD_PUBLIC_AS_RELEASE)
        SET_TARGET_PROPERTIES(${TargetName} PROPERTIES RELEASE_POSTFIX ".public")
    ENDIF()

ENDMACRO(N_SET_EXE_LINKER_DEFAULTS)

#-------------------------------------------------------------------------------
# Sets up the default compiler options for a Nebula library target or executable.
#-------------------------------------------------------------------------------

MACRO(N_SET_COMPILER_DEFAULTS Target)

    IF(MSVC)
		IF(N_STATIC_BUILD)
			SET(N_RT_TYPE "MT")
		ELSE()
			SET(N_RT_TYPE "MD")
		ENDIF()
		
		TARGET_COMPILE_DEFINITIONS(${Target} PRIVATE __WIN32__ WIN32 _HAS_EXCEPTIONS=0 $<$<CONFIG:Debug>:_DEBUG> $<$<CONFIG:Release>:NDEBUG PUBLIC_BUILD> $<$<CONFIG:RelWithDebInfo>:NDEBUG> 
													 $<$<BOOL:${CMAKE_CL_64}>:WIN64 __WIN64__>													 
													 ${N_DEFINITIONS}
		)
		
		TARGET_COMPILE_OPTIONS(${Target} PRIVATE /GF /W3 /fp:fast /WX /Oi /arch:SSE /arch:SSE2 /nologo /errorReport:prompt  /wd4091 /wd4267 /wd4996 /wd4018)
		TARGET_COMPILE_OPTIONS(${Target} PRIVATE $<$<CONFIG:Debug>:/Od /Gm /RTC1 /${N_RT_TYPE}d /Gy /Zi> $<$<CONFIG:Release>:/Ob2 /Oi /Ot /Oy /MP ${N_GL} /FD /${N_RT_TYPE} /GS->)
		TARGET_COMPILE_OPTIONS(${Target} PRIVATE $<$<CONFIG:RelWithDebInfo>:/Ob2 /Oi /Ot /Oy- /MP ${N_GL} /FD /${N_RT_TYPE} /GS- /Zi>)		
		if(${ARGC} EQUAL 1)		
			TARGET_LINK_LIBRARIES(${Target} PUBLIC NebulaConfig)		
		endif()
			

		SET(CMAKE_EXE_LINKER_FLAGS "/ignore:4099")
    ELSEIF(UNIX)
		TARGET_COMPILE_DEFINITIONS(${Target} PRIVATE ${N_DEFINITIONS})
		TARGET_COMPILE_OPTIONS(${Target} PRIVATE -Wno-switch-bool -fpermissive -std=gnu++0x -Wno-multichar -Werror -msse4.2 -march=sandybridge -ffast-math -fno-trapping-math -funsafe-math-optimizations -ffinite-math-only -mrecip=all)
		TARGET_COMPILE_OPTIONS(${Target} PRIVATE $<$<CONFIG:Debug>:-D_DEBUG>)
	ENDIF()
	EXPORT(TARGETS ${Target}  APPEND FILE ${CMAKE_BINARY_DIR}/nebula-exports.cmake)
ENDMACRO(N_SET_COMPILER_DEFAULTS)

MACRO(N_CREATE_FEATURE featurename files nidls)
	SET(filevar ${${files}})
	N_WRAP_ADD_PROJ_NIDL_FILES(${featurename} filevar ${${nidls}})
	ADD_LIBRARY(${featurename} STATIC ${filevar})	
	SET_TARGET_PROPERTIES(${featurename} PROPERTIES FOLDER "N3SDK/N3/features")	
ENDMACRO()

MACRO(PARSE_PROJ_NIDLS name projfile)

FILE(READ ${projfile} prjinf)
string(REGEX MATCH "<NIDL>.*</NIDL>" nidbl ${prjinf})

string(REGEX MATCHALL "<File name=\"[^\"]+\"/>" foo ${nidbl})
SET(proj_nidls)
FOREACH(entry ${foo})
STRING(REGEX REPLACE "<File name=\"([^\"]+)\"/>" "\\1" outf ${entry})
LIST(APPEND proj_nidls ${outf})
ENDFOREACH()
N_WRAP_NIDL_FILES(${name} ${proj_nidls})
ENDMACRO()