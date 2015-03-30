#-------------------------------------------------------------------------------
# Toolkit
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/application
                    ${Nebula3_SOURCE_DIR}/code/addons
                    ${Nebula3_SOURCE_DIR}/code/application/basegamefeature
                    ${Nebula3_SOURCE_DIR}/code/application/corefeature
                    ${Nebula3_SOURCE_DIR}/code/application/graphicsfeature
                    ${Nebula3_SOURCE_DIR}/code/application/physicsfeature
                    ${Nebula3_SOURCE_DIR}/code/extlibs
					${Nebula3_SOURCE_DIR}/code/extlibs/DevIL/DevIL/include
                    ${Nebula3_SOURCE_DIR}/code/extlibs/fbx
                    ${Nebula3_SOURCE_DIR}/code/foundation
                    ${Nebula3_SOURCE_DIR}/code/render
                    ${Nebula3_SOURCE_DIR}/code/sui
                    ${Nebula3_SOURCE_DIR}/code/tools
                    ${Nebula3_SOURCE_DIR}/code/toolkit
                    ${Nebula3_SOURCE_DIR}/code/toolkit/toolkitutil
                    ${Nebula3_SOURCE_DIR}/code/extlibs/recastnavigation/Recast/Include
                    ${Nebula3_SOURCE_DIR}/code/extlibs/recastnavigation/Detour/Include
                    ${Nebula3_SOURCE_DIR}/code/extlibs/nvidia-texture-tools/src
                    ${Nebula3_SOURCE_DIR}/code/extlibs/nvidia-texture-tools/extern/poshlib
                    ${CMAKE_BINARY_DIR}/code/extlibs/nvidia-texture-tools/src
                    ${Nebula3_SOURCE_DIR}/code                    
                    ${Physics_SOURCE_DIR}
)


IF(NOT N_BUILD_FIRSTTIME)

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCST ${SRC} ${HEAD})
SOURCE_GROUP("toolkitutil" FILES ${SRCST})

FILE(GLOB SRC "animutil/*.cc")
FILE(GLOB HEAD "animutil/*.h")
LIST(APPEND SRCSA ${SRC} ${HEAD})
SOURCE_GROUP("toolkitutil\\animutil" FILES ${SRCSA})

FILE(GLOB SRC "meshutil/*.cc")
FILE(GLOB HEAD "meshutil/*.h")
LIST(APPEND SRCSM ${SRC} ${HEAD})
SOURCE_GROUP("toolkitutil\\meshutil" FILES ${SRCSM})

FILE(GLOB SRC "n2util/*.cc")
FILE(GLOB HEAD "n2util/*.h")
LIST(APPEND SRCSN ${SRC} ${HEAD})
SOURCE_GROUP("toolkitutil\\n2util" FILES ${SRCSN})

FILE(GLOB SRC "n3util/*.cc")
FILE(GLOB HEAD "n3util/*.h")
LIST(APPEND SRCSN ${SRC} ${HEAD})
SOURCE_GROUP("toolkitutil\\n3util" FILES ${SRCSN})

FILE(GLOB SRC "posteffect/*.cc")
FILE(GLOB HEAD "posteffect/*.h")

LIST(APPEND SRCSPFX ${SRC} ${HEAD})
SOURCE_GROUP("toolkitutil\\posteffect" FILES ${SRCSPFX})

SET(SRCSX            
			texutil/cubefilterer.cc
			texutil/cubefilterer.h
            texutil/textureattrtable.cc
            texutil/imageconverter.h
            texutil/imageconverter.cc
            texutil/textureconverter.h
            texutil/textureconversionjob.cc
            texutil/textureconversionjob.h
            texutil/textureattrs.cc
            texutil/textureconverter.cc
            texutil/textureattrtable.h                                    
            texutil/textureattrs.h)
            
IF(N_BUILD_NVTT)
	SET(SRCSX_PL texutil/nvtttextureconversionjob.cc
                 texutil/nvtttextureconversionjob.h)
	ADD_DEFINITIONS(-DUSE_NVTT)
ELSE()
    SET(SRCSX_PL texutil/win32textureconversionjob.cc
                 texutil/win32textureconversionjob.h)
ENDIF()                 
                 
LIST(APPEND SRCSX ${SRCSX_PL})
SOURCE_GROUP("toolkitutil\\texutil" FILES ${SRCSX})

FILE(GLOB SRC "base/*.cc")
FILE(GLOB HEAD "base/*.h")
LIST(APPEND SRCSB ${SRC} ${HEAD})
SOURCE_GROUP("toolkitutil\\base" FILES ${SRCSB})

FILE(GLOB SRC "fbx/*.cc")
FILE(GLOB HEAD "fbx/*.h")
LIST(APPEND SRCSF ${SRC} ${HEAD})
SOURCE_GROUP("toolkitutil\\fbx" FILES ${SRCSF})

FILE(GLOB SRC "fbx/helpers/*.cc")
FILE(GLOB HEAD "fbx/helpers/*.h")
LIST(APPEND SRCSFS ${SRC} ${HEAD})
SOURCE_GROUP("toolkitutil\\fbx\\helpers" FILES ${SRCSFS})

FILE(GLOB SRC "fbx/node/*.cc")
FILE(GLOB HEAD "fbx/node/*.h")
LIST(APPEND SRCSND ${SRC} ${HEAD})
SOURCE_GROUP("toolkitutil\\fbx\\node" FILES ${SRCSND})

FILE(GLOB SRC "fbx/character/*.cc")
FILE(GLOB HEAD "fbx/character/*.h")
LIST(APPEND SRCSND ${SRC} ${HEAD})
SOURCE_GROUP("toolkitutil\\fbx\\character" FILES ${SRCSND})

FILE(GLOB SRC "game/*.cc")
FILE(GLOB HEAD "game/*.h")
LIST(APPEND SRCSG ${SRC} ${HEAD})
SOURCE_GROUP("toolkitutil\\game" FILES ${SRCSG})

FILE(GLOB SRC "gui/*.cc")
FILE(GLOB HEAD "gui/*.h")
LIST(APPEND SRCSGUI ${SRC} ${HEAD})
SOURCE_GROUP("toolkitutil\\gui" FILES ${SRCSGUI})

FILE(GLOB SRC "modelutil/*.cc")
FILE(GLOB HEAD "modelutil/*.h")
LIST(APPEND SRCSMOD ${SRC} ${HEAD})
SOURCE_GROUP("toolkitutil\\modelutil" FILES ${SRCSMOD})

FILE(GLOB SRC "idldocument/*.cc")
FILE(GLOB HEAD "idldocument/*.h")
LIST(APPEND SRCSIDL ${SRC} ${HEAD})
SOURCE_GROUP("toolkitutil\\idl" FILES ${SRCSIDL})

IF(MSVC)       
    SET(SRCAPPLAUNCH win32/win32applauncher.cc win32/win32applauncher.h)
    SOURCE_GROUP("toolkitutil\\win32" FILES ${SRCAPPLAUNCH})
ELSE()
    SET(SRCAPPLAUNCH posix/posixapplauncher.cc posix/posixapplauncher.h)
    SOURCE_GROUP("toolkitutil\\posix" FILES ${SRCAPPLAUNCH})    
ENDIF()

FILE(GLOB SRC "../distributedtools/*.cc")
FILE(GLOB HEAD "../distributedtools/*.h")
LIST(APPEND SRCSD ${SRC} ${HEAD})
SOURCE_GROUP("distributedtools" FILES ${SRCSD})
FILE(GLOB SRC "../distributedtools/distributedjobs/*.cc")
FILE(GLOB HEAD "../distributedtools/distributedjobs/*.h")
LIST(APPEND SRCSDD ${SRC} ${HEAD})
SOURCE_GROUP("distributedtools\\Jobs" FILES ${SRCSDD})

ELSE()
FILE(GLOB SRC "idldocument/*.cc")
FILE(GLOB HEAD "idldocument/*.h")
LIST(APPEND SRCSIDL ${SRC} ${HEAD})
SOURCE_GROUP("toolkitutil\\idl" FILES ${SRCSIDL})
ENDIF()
set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})

LIST(APPEND ALLSRCS ${SRCST} ${SRCSA} ${SRCSM}  ${SRCSPFX} ${SRCSN} ${SRCSX}  ${SRCSG} ${SRCSGUI}  ${SRCSB}  ${SRCSF} ${SRCSFS} ${SRCSND}  ${SRCSDD} ${SRCSD} ${SRCSMOD} ${SRCSIDL} ${SRCSREC} ${PCH_files} ${SRCAPPLAUNCH})

ADD_DEFINITIONS(-DNVTT_EXPORTS)

add_library(toolkitutil STATIC ${ALLSRCS} )
SET_TARGET_PROPERTIES(toolkitutil PROPERTIES FOLDER "N3SDK/Toolkit")
N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(toolkitutil stdneb.h stdneb.cc ../)
ENDIF()
