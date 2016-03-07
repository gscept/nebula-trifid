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
#------------------------------------------------------------------------------
# macro for qt projects
#------------------------------------------------------------------------------

MACRO(N_SET_QT_PROPERTIES inTarget inFolder)
        SET_TARGET_PROPERTIES(${inTarget} PROPERTIES FOLDER ${inFolder})
        SET_TARGET_PROPERTIES(${inTarget} PROPERTIES DEBUG_POSTFIX ".debug")
        
        IF(NOT N_BUILD_PUBLIC_AS_RELEASE)
                SET_TARGET_PROPERTIES(${inTarget} PROPERTIES RELEASE_POSTFIX ".public")
        ENDIF()
        
        SET_TARGET_PROPERTIES(${inTarget} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECTBINDIR})
        SET_TARGET_PROPERTIES(${inTarget} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECTBINDIR})
        SET_TARGET_PROPERTIES(${inTarget} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECTBINDIR})
        SET_TARGET_PROPERTIES(${inTarget} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECTBINDIR})
        SET_TARGET_PROPERTIES(${inTarget} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECTBINDIR})
        SET_TARGET_PROPERTIES(${inTarget} PROPERTIES PDB_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/pdb)
        SET_TARGET_PROPERTIES(${inTarget} PROPERTIES PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pdb)
        SET_TARGET_PROPERTIES(${inTarget} PROPERTIES PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/pdb)
        SET_TARGET_PROPERTIES(${inTarget} PROPERTIES PDB_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/pdb)
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

MACRO(N_SET_COMPILER_DEFAULTS)

    IF(MSVC)
			SET(CMAKE_EXE_LINKER_FLAGS "/ignore:4099")
                IF(CMAKE_CL_64)
                # C++ compiler
                        SET(CMAKE_CXX_FLAGS 
                                "/D__WIN32__ /DWIN32 /DWIN64 /D__WIN64__ /D_HAS_EXCEPTIONS=0 /GF /fp:fast /W3 /WX /nologo /errorReport:prompt /wd4091" 
                                CACHE STRING "Flags used by the compiler during all build types." FORCE)
                        SET(CMAKE_CXX_FLAGS_DEBUG
                                "/D_DEBUG /Od /Gm /RTC1 /MTd /Gy /Zi"
                                CACHE STRING "Flags used by the compiler during debug builds." FORCE)
                        SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO
                                "/Ob2 /Oi /Os /Oy ${N_GL} /FD /MT /GS- /Zi /MP /DNDEBUG"
                                CACHE STRING "Flags used by the compiler during release builds." FORCE)
                        SET(CMAKE_CXX_FLAGS_RELEASE
                                "/Ob2 /Oi /Os /Oy ${N_GL} /FD /MT /GS- /MP /DNDEBUG"
                                CACHE STRING "Flags used by the compiler during release builds." FORCE)
                        # C compiler (force compilation of .c files using C++ compiler)
                        SET(CMAKE_C_FLAGS 
                                "/D__WIN32__ /DWIN32 /DWIN64 /D__WIN64__ /D_HAS_EXCEPTIONS=0 /GF /fp:fast /W3 /WX /nologo /errorReport:prompt /wd4091" 
                                CACHE STRING "Flags used by the compiler during all build types." FORCE)
                        SET(CMAKE_C_FLAGS_DEBUG
                                "/D_DEBUG /Od /Gm /RTC1 /MTd /Gy /Zi"
                                CACHE STRING "Flags used by the compiler during debug builds." FORCE)
                        SET(CMAKE_C_FLAGS_RELWITHDEBINFO
                                "/Ob2 /Oi /Os /Oy ${N_GL} /FD /MT /GS- /Zi /MP /DNDEBUG"
                                CACHE STRING "Flags used by the compiler during release builds." FORCE)
                        SET(CMAKE_C_FLAGS_RELEASE
                                "/Ob2 /Oi /Os /Oy ${N_GL} /FD /MT /GS- /MP /DNDEBUG"
                                CACHE STRING "Flags used by the compiler during release builds." FORCE)		
                ELSE()
                        # C++ compiler		
                        SET(CMAKE_CXX_FLAGS 
                                "/D__WIN32__ /DWIN32 /D_HAS_EXCEPTIONS=0 /GF /fp:fast /W3 /WX /MP /Oi /arch:SSE /arch:SSE2 /nologo /errorReport:prompt /wd4091" 
                                CACHE STRING "Flags used by the compiler during all build types." FORCE)
                        SET(CMAKE_CXX_FLAGS_DEBUG
                                "/D_DEBUG /Od /RTC1 /MTd /Gy /ZI"
                                CACHE STRING "Flags used by the compiler during debug builds." FORCE)
                        SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO
                                "/Ob2 /Oi /Ot /Oy ${N_GL} /FD /MT /GS- /Zi /DNDEBUG"
                                CACHE STRING "Flags used by the compiler during release builds." FORCE)
                        SET(CMAKE_CXX_FLAGS_RELEASE
                                "/Ob2 /Oi /Ot /Oy ${N_GL} /FD /MT /GS- /DPUBLIC_BUILD /DNDEBUG"
                                CACHE STRING "Flags used by the compiler during release builds." FORCE)
                        # C compiler (force compilation of .c files using C++ compiler)
                        SET(CMAKE_C_FLAGS 
                                "/D__WIN32__ /DWIN32 /D_HAS_EXCEPTIONS=0 /GF /fp:fast /W3 /WX /MP /nologo /Oi /arch:SSE /arch:SSE2 /errorReport:prompt /wd4091" 
                                CACHE STRING "Flags used by the compiler during all build types." FORCE)
                        SET(CMAKE_C_FLAGS_DEBUG
                                "/D_DEBUG /Od /RTC1 /MTd /Gy /ZI"
                                CACHE STRING "Flags used by the compiler during debug builds." FORCE)
                        SET(CMAKE_C_FLAGS_RELWITHDEBINFO
                                "/Ob2 /Oi /Ot /Oy ${N_GL} /FD /MT /GS- /Zi /DNDEBUG"
                                CACHE STRING "Flags used by the compiler during release builds." FORCE)
                        SET(CMAKE_C_FLAGS_RELEASE
                                "/Ob2 /Oi /Ot /Oy ${N_GL} /FD /MT /GS- /DPUBLIC_BUILD /DNDEBUG"
                                CACHE STRING "Flags used by the compiler during release builds." FORCE)
                ENDIF()
    ELSEIF(UNIX)
                # C compiler
                # remove multichar warning spewage
                SET(CMAKE_C_FLAGS "-Wno-multichar -Werror -msse4.2 -march=sandybridge -ffast-math -fno-trapping-math -funsafe-math-optimizations -ffinite-math-only -mrecip=all")
                SET(CMAKE_C_FLAGS_DEBUG "-g -D_DEBUG")
                SET(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -O2 -D_DEBUG")
                SET(CMAKE_C_FLAGS_RELEASE " -O2 -Og")
        
                # C++ compiler
                # remove multichar warning spewage
                SET(CMAKE_CXX_FLAGS "-Wno-multichar -fpermissive -std=gnu++0x -msse4.2 -march=sandybridge -ffast-math -fno-trapping-math -funsafe-math-optimizations -ffinite-math-only -mrecip=all")
                SET(CMAKE_CXX_FLAGS_DEBUG "-g -D_DEBUG")
                SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2 -Og -D_DEBUG")
                SET(CMAKE_CXX_FLAGS_RELEASE " -O2 -Og -g")
	ENDIF()
	
	ADD_DEFINITIONS(${N_DEFINITIONS})
ENDMACRO(N_SET_COMPILER_DEFAULTS)

#-------------------------------------------------------------------------------
# copies of qt macros that add pch flags
#-------------------------------------------------------------------------------

IF(WIN32)
	MACRO (PCH_QT4_WRAP_CPP outfiles )
		# get include dirs
		QT4_GET_MOC_FLAGS(moc_flags)
		QT4_EXTRACT_OPTIONS(moc_files moc_options moc_target ${ARGN})

		FOREACH (it ${moc_files})
			GET_FILENAME_COMPONENT(it ${it} ABSOLUTE)
			QT4_MAKE_OUTPUT_FILE(${it} moc_ cxx outfile)
			set (moc_flags_append "-fstdneb.h" "-f${it}") # pch hack.
			QT4_CREATE_MOC_COMMAND(${it} ${outfile} "${moc_flags};${moc_flags_append}" "${moc_options}" "${moc_target}")
			SET(${outfiles} ${${outfiles}} ${outfile})
		ENDFOREACH(it)
	ENDMACRO (PCH_QT4_WRAP_CPP)
	
	macro (NONPCH_QT4_ADD_RESOURCES outfiles )
		QT4_EXTRACT_OPTIONS(rcc_files rcc_options rcc_target ${ARGN})

		foreach (it ${rcc_files})
		get_filename_component(outfilename ${it} NAME_WE)
		get_filename_component(infile ${it} ABSOLUTE)
		get_filename_component(rc_path ${infile} PATH)
		set(outfile ${CMAKE_CURRENT_BINARY_DIR}/qrc_${outfilename}.cxx)

		set(_RC_DEPENDS)
		if(EXISTS "${infile}")
		  #  parse file for dependencies
		  #  all files are absolute paths or relative to the location of the qrc file
		  file(READ "${infile}" _RC_FILE_CONTENTS)
		  string(REGEX MATCHALL "<file[^<]+" _RC_FILES "${_RC_FILE_CONTENTS}")
		  foreach(_RC_FILE ${_RC_FILES})
			string(REGEX REPLACE "^<file[^>]*>" "" _RC_FILE "${_RC_FILE}")
			if(NOT IS_ABSOLUTE "${_RC_FILE}")
			  set(_RC_FILE "${rc_path}/${_RC_FILE}")
			endif()
			set(_RC_DEPENDS ${_RC_DEPENDS} "${_RC_FILE}")
		  endforeach()
		  unset(_RC_FILES)
		  unset(_RC_FILE_CONTENTS)
		  # Since this cmake macro is doing the dependency scanning for these files,
		  # let's make a configured file and add it as a dependency so cmake is run
		  # again when dependencies need to be recomputed.
		  QT4_MAKE_OUTPUT_FILE("${infile}" "" "qrc.depends" out_depends)
		  configure_file("${infile}" "${out_depends}" COPYONLY)
		else()
		  # The .qrc file does not exist (yet). Let's add a dependency and hope
		  # that it will be generated later
		  set(out_depends)
		endif()	
		add_custom_command(OUTPUT ${outfile}
		  COMMAND ${QT_RCC_EXECUTABLE}
		  ARGS ${rcc_options} -name ${outfilename} -o ${outfile} ${infile}
		  MAIN_DEPENDENCY ${infile}
		  DEPENDS ${_RC_DEPENDS} "${out_depends}" VERBATIM)
		set(${outfiles} ${${outfiles}} ${outfile})
		if(MSVC)
			SET_SOURCE_FILES_PROPERTIES(${outfile} PROPERTIES COMPILE_FLAGS /Y-)
		endif()
	  endforeach ()	
	endmacro ()
ELSEIF(UNIX)
	macro (PCH_QT4_WRAP_CPP outfiles )
		# get include dirs
		QT4_GET_MOC_FLAGS(moc_flags)
		QT4_EXTRACT_OPTIONS(moc_files moc_options moc_target ${ARGN})

		foreach (it ${moc_files})
			get_filename_component(it ${it} ABSOLUTE)
			QT4_MAKE_OUTPUT_FILE(${it} moc_ cxx outfile)
			set (moc_flags_append "-fstdneb.h" "-f${it}") # pch hack.
			QT4_CREATE_MOC_COMMAND(${it} ${outfile} "${moc_flags}" "${moc_options};${moc_flags_append}" "${moc_target}")
			set(${outfiles} ${${outfiles}} ${outfile})
		endforeach()
	endmacro ()
	
	MACRO (NONPCH_QT4_ADD_RESOURCES outfiles )
		QT4_EXTRACT_OPTIONS(rcc_files rcc_options rcc_target ${ARGN})

		foreach (it ${rcc_files})
		get_filename_component(outfilename ${it} NAME_WE)
		get_filename_component(infile ${it} ABSOLUTE)
		get_filename_component(rc_path ${infile} PATH)
		set(outfile ${CMAKE_CURRENT_BINARY_DIR}/qrc_${outfilename}.cxx)

		set(_RC_DEPENDS)
		if(EXISTS "${infile}")
		  #  parse file for dependencies
		  #  all files are absolute paths or relative to the location of the qrc file
		  file(READ "${infile}" _RC_FILE_CONTENTS)
		  string(REGEX MATCHALL "<file[^<]+" _RC_FILES "${_RC_FILE_CONTENTS}")
		  foreach(_RC_FILE ${_RC_FILES})
			string(REGEX REPLACE "^<file[^>]*>" "" _RC_FILE "${_RC_FILE}")
			if(NOT IS_ABSOLUTE "${_RC_FILE}")
			  set(_RC_FILE "${rc_path}/${_RC_FILE}")
			endif()
			set(_RC_DEPENDS ${_RC_DEPENDS} "${_RC_FILE}")
		  endforeach()
		  unset(_RC_FILES)
		  unset(_RC_FILE_CONTENTS)
		  # Since this cmake macro is doing the dependency scanning for these files,
		  # let's make a configured file and add it as a dependency so cmake is run
		  # again when dependencies need to be recomputed.
		  QT4_MAKE_OUTPUT_FILE("${infile}" "" "qrc.depends" out_depends)
		  configure_file("${infile}" "${out_depends}" COPYONLY)
		else()
		  # The .qrc file does not exist (yet). Let's add a dependency and hope
		  # that it will be generated later
		  set(out_depends)
		endif()

		add_custom_command(OUTPUT ${outfile}
		  COMMAND ${QT_RCC_EXECUTABLE}
		  ARGS ${rcc_options} -name ${outfilename} -o ${outfile} ${infile}
		  MAIN_DEPENDENCY ${infile}
		  DEPENDS ${_RC_DEPENDS} "${out_depends}" VERBATIM)
		set(${outfiles} ${${outfiles}} ${outfile})
	  endforeach ()
	endmacro (NONPCH_QT4_ADD_RESOURCES)
	SET(QT_LIBS "")
	#SET(QT_QMAKE_EXECUTABLE /usr/bin/qmake)
	#SET(QT_MOC_EXECUTABLE /usr/bin/moc)
	#SET(QT_RCC_EXECUTABLE /usr/bin/rcc)
	#SET(QT_UIC_EXECUTABLE /usr/bin/uic)
        
ENDIF()

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