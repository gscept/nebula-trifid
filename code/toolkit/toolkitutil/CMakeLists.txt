#-------------------------------------------------------------------------------
# Toolkit
#-------------------------------------------------------------------------------

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
			

LIST(APPEND SRCSX texutil/cubefilterer.cc texutil/cubefilterer.h)

            
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

SET(SRCSG 
	game/gameexporter.cc
	game/gameexporter.h
	game/leveldbwriter.cc
	game/leveldbwriter.h
	game/levelparser.cc
	game/levelparser.h
	)

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

add_library(toolkitutil STATIC ${ALLSRCS} )
IF(N_BUILD_FIRSTTIME)
    TARGET_LINK_LIBRARIES(toolkitutil foundation)
ELSE()
    TARGET_LINK_LIBRARIES(toolkitutil foundation render application sqlite compiler editorfeatures ${FBX_LIBRARY} $<$<BOOL:${N_BUILD_NVTT}>:nvcore nvtt nvimage nvmath posh squish nvthread bc6h bc7>)
ENDIF()
TARGET_COMPILE_DEFINITIONS(toolkitutil PUBLIC $<$<BOOL:${N_BUILD_NVTT}>:NVTT_EXPORTS> FBXSDK_NEW_API)
TARGET_INCLUDE_DIRECTORIES(toolkitutil PUBLIC ${Nebula3_SOURCE_DIR}/code/toolkit ${Nebula3_SOURCE_DIR}/code/toolkit/toolkitutil)
SET_TARGET_PROPERTIES(toolkitutil PROPERTIES FOLDER "N3SDK/Toolkit")
N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(toolkitutil stdneb.h stdneb.cc ../)
ENDIF()
