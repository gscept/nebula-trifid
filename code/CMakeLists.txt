

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
		SET(N_DEFINITIONS ${N_DEFINITIONS} -D__OGL4__ -DGLEW_STATIC -D__ANYFX__  -DIL_STATIC_LIB)
ENDIF()

IF(N_BUILD_LIBROCKET)
	SET(N_DEFINITIONS ${N_DEFINITIONS} -D__LIBROCKET__)
ELSEIF(N_BUILD_CEUI)
	SET(N_DEFINITIONS ${N_DEFINITIONS} -D__CEUI__)
ENDIF()

IF(N_BUILD_BULLET)
	SET(N_DEFINITIONS ${N_DEFINITIONS} -D__USE_BULLET__)
	SET(PHYSICS_DEPENDENCIES  BulletDynamics BulletCollision LinearMath) #HACD)
ENDIF()
IF(N_BUILD_PHYSX)
	SET(N_DEFINITIONS ${N_DEFINITIONS} -D__USE_PHYSX__)
ENDIF()
IF(N_BUILD_HAVOK)
	SET(N_DEFINITIONS ${N_DEFINITIONS} -D__USE_HAVOK__)
	FIND_PATH(HAVOC_DIR "Havok Limited Use License Agreement for PC XS 12-19-2011.txt"
				${N3ROOT}/libs/contrib/hk2011_3_1_r1)
				
	SET(Havok_SOURCE_DIR ${HAVOC_DIR}/Source)
	
	INCLUDE_DIRECTORIES(${Havok_SOURCE_DIR}	)
    
	if(MSVC90)
		SET(Havok_LIB_DIR ${HAVOC_DIR}/Lib/win32_net_9-0)
	ELSEIF(MSVC10)
		SET(Havok_LIB_DIR ${HAVOC_DIR}/Lib/win32_vs2010)
	ENDIF()
	
    # add the option to choose fulldebug/debug libraries
    SET(HAVOK_DEFAULT_DEBUG_LIBS "fulldebug_multithreaded")
    SET(HAVOK_DEBUG_LIBS ${HAVOK_DEFAULT_DEBUG_LIBS} CACHE STRING "Fulldebug supposedly allows debugging the havok source, but it produces hundreds of warnings when building in VS. See 1.5.4 Linking with Havok in the Quickstart Guide documentation for details about the fulldebug/debug libraries.")
    SET_PROPERTY(CACHE HAVOK_DEBUG_LIBS PROPERTY STRINGS "debug_multithreaded" "fulldebug_multithreaded")
    SET(${HAVOK_DEBUG_LIBS} ON)
    
	SET(Havok_dbg_lib ${Havok_LIB_DIR}/${HAVOK_DEBUG_LIBS})
    SET(Havok_rel_lib ${Havok_LIB_DIR}/release_multithreaded)
	
	#FIXME this is fairly fugly but well, dont see any other way at the moment
	
	SET(N_LIBS ${N_LIBS} 
	optimized ${Havok_rel_lib}/hctSdkUtils.lib
	debug ${Havok_dbg_lib}/hctSdkUtils.lib
	optimized ${Havok_rel_lib}/hkaAnimation.lib
	debug ${Havok_dbg_lib}/hkaAnimation.lib
	optimized ${Havok_rel_lib}/hkaInternal.lib
	debug ${Havok_dbg_lib}/hkaInternal.lib
	optimized ${Havok_rel_lib}/hkaRagdoll.lib
	debug ${Havok_dbg_lib}/hkaRagdoll.lib
	optimized ${Havok_rel_lib}/hkBase.lib
	debug ${Havok_dbg_lib}/hkBase.lib
	optimized ${Havok_rel_lib}/hkcdCollide.lib
	debug ${Havok_dbg_lib}/hkcdCollide.lib
	optimized ${Havok_rel_lib}/hkcdInternal.lib
	debug ${Havok_dbg_lib}/hkcdInternal.lib
	optimized ${Havok_rel_lib}/hkCompat.lib
	debug ${Havok_dbg_lib}/hkCompat.lib
	optimized ${Havok_rel_lib}/hkgBridge.lib
	debug ${Havok_dbg_lib}/hkgBridge.lib
	optimized ${Havok_rel_lib}/hkgCommon.lib	
	debug ${Havok_dbg_lib}/hkgCommon.lib	
	
	optimized ${Havok_rel_lib}/hkGeometryUtilities.lib
	debug ${Havok_dbg_lib}/hkGeometryUtilities.lib
	optimized ${Havok_rel_lib}/hkInternal.lib
	debug ${Havok_dbg_lib}/hkInternal.lib
	optimized ${Havok_rel_lib}/hkpCollide.lib
	debug ${Havok_dbg_lib}/hkpCollide.lib
	optimized ${Havok_rel_lib}/hkpConstraintSolver.lib
	debug ${Havok_dbg_lib}/hkpConstraintSolver.lib
	optimized ${Havok_rel_lib}/hkpDynamics.lib
	debug ${Havok_dbg_lib}/hkpDynamics.lib
	optimized ${Havok_rel_lib}/hkpInternal.lib
	debug ${Havok_dbg_lib}/hkpInternal.lib
	optimized ${Havok_rel_lib}/hkpUtilities.lib
	debug ${Havok_dbg_lib}/hkpUtilities.lib
	optimized ${Havok_rel_lib}/hkpVehicle.lib
	debug ${Havok_dbg_lib}/hkpVehicle.lib
	optimized ${Havok_rel_lib}/hkSceneData.lib
	debug ${Havok_dbg_lib}/hkSceneData.lib
	optimized ${Havok_rel_lib}/hkSerialize.lib
	debug ${Havok_dbg_lib}/hkSerialize.lib
	optimized ${Havok_rel_lib}/hkVisualize.lib
	debug ${Havok_dbg_lib}/hkVisualize.lib
	)

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
                db attr posteffect_NIDLS render_visibilityboxsystemjob render_visibilityquadtreejob
				imgui dynui
                render_animjobsamplemix render_animjobsample render_particlejob render_charevalskeletonjob ${PHYSICS_DEPENDENCIES}
                foundation_NIDLS 
                render_NIDLS ui scripting scripting_NIDLS lua effects effects_NIDLS forest forest_NIDLS
                vibration vibration_NIDLS                   
                foundation
                foundation_NIDLS base64
                tinyxml zlib sqlite ui_NIDLS
				multiplayer multiplayer_NIDLS RakNetLibStatic upnpc-static jansson StackWalker
                ${PLATFORM_PROJECTS} ${N_RENDER_LIBS}
                ${N_LIBS}
                ${ROCKET}
)


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
            "/MANIFEST:NO /INCREMENTAL:NO /LTCG /DEBUG"
            CACHE STRING "Flags used by the linker when building an executable." FORCE
        )
		
		SET(CMAKE_EXE_LINKER_FLAGS_RELEASE
            "/MANIFEST:NO /INCREMENTAL:NO /LTCG"
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


#------------------------------------------------------------------------------
# macro for nidl files
#------------------------------------------------------------------------------

MACRO(N_WRAP_NIDL_FILES proj)		
        set(files ${ARGN})
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
                                        )
                ELSEIF(UNIX)
                    ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_BINARY_DIR}/nidl/${outfile} ${CMAKE_BINARY_DIR}/nidl/${outfileh}
                                        COMMAND ${N3ROOT}/bin/posix/idlc.toolkit -output ${CMAKE_BINARY_DIR}/nidl/ ${nidl}
                                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
                                        MAIN_DEPENDENCY ${nidl}
                                        )
                ENDIF()				
                SOURCE_GROUP("Generated" FILES "${CMAKE_BINARY_DIR}/nidl/${outfile}" "${CMAKE_BINARY_DIR}/nidl/${outfileh}")
                List(APPEND outlist "${CMAKE_BINARY_DIR}/nidl/${outfile}" "${CMAKE_BINARY_DIR}/nidl/${outfileh}")
        ENDFOREACH()
        ADD_LIBRARY(${proj} STATIC ${outlist})
        SET_TARGET_PROPERTIES(${proj} PROPERTIES FOLDER "NIDL")
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
# The individual targets that make up Nebula3
#-------------------------------------------------------------------------------

IF(N_BUILD_QT AND NOT N_BUILD_FIRSTTIME)

IF(WIN32)
OPTION(N_USE_SYSTEM_QT "Use Qt provided by system" On)

if(NOT N_USE_SYSTEM_QT)
        SET(QT_QMAKE_EXECUTABLE ${N3ROOT}/libs/contrib/qt/bin/qmake.exe)
        SET(QT_LIBS "imm32.lib" "winmm.lib" "Ws2_32.lib")
        SET(QTTOOLS ${N3ROOT}/code/qttoolkit/qttools)
        SET(QTVERSION "4.8.4")
        SET(QT_MKSPECS_DIR ${N3ROOT}/libs/contrib/qt/mkspecs)            
        SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
                ${N3ROOT}/libs/contrib/qt/cmake)
        SET(N3QT ${N3ROOT}/libs/contrib/qt/)
		FIND_PACKAGE(N3Qt4 REQUIRED)
else()
	FIND_PACKAGE(Qt4 REQUIRED QtCore QtGui QtXml QtNetwork)
	
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
		  configure_file("${infile}" "${out_depends}" COPY_ONLY)
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
ENDIF()

ELSEIF(UNIX)

        # linux doesnt need the precompiled hax
#        MACRO (PCH_QT4_WRAP_CPP outfiles )
        # get include dirs
#        QT4_GET_MOC_FLAGS(moc_flags)
#        QT4_EXTRACT_OPTIONS(moc_files moc_options moc_target ${ARGN})
#
#        FOREACH (it ${moc_files})
#                GET_FILENAME_COMPONENT(it ${it} ABSOLUTE)
#                QT4_MAKE_OUTPUT_FILE(${it} moc_ cxx outfile) 
#                        set (moc_flags_append "-fstdneb.h" "-f${it}") # pch hack.
#                QT4_CREATE_MOC_COMMAND(${it} ${outfile} "${moc_flags} ${moc_flags_append}" "${moc_options} ${moc_target}")
#                SET(${outfiles} ${${outfiles}} ${outfile})
#        ENDFOREACH(it)
	
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
      configure_file("${infile}" "${out_depends}" COPY_ONLY)
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
        FIND_PACKAGE(Qt4 REQUIRED)
        
ENDIF()
        
        
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

