#-------------------------------------------------------------------------------
# idlc
#-------------------------------------------------------------------------------

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
LIST(APPEND SRCS ${SRC} ${HEAD})
SOURCE_GROUP("idlc" FILES ${SRCS})
	
set(PCH_files	
    ../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${PCH_files})
ADD_EXECUTABLE(idlc ${SRCS} ${SRCSIDL} ${PCH_files})
SET_TARGET_PROPERTIES(idlc PROPERTIES FOLDER "N3SDK/Toolkit")


N_SET_COMPILER_DEFAULTS()
N_SET_EXE_LINKER_DEFAULTS(idlc)
N_SET_EXE_SUBSYSTEM(idlc Console)
IF(N_USE_PRECOMPILED_HEADERS)
	N_SET_PCH(idlc stdneb.h stdneb.cc ../)
ENDIF()

ADD_DEPENDENCIES(idlc foundation idldocument)
TARGET_LINK_LIBRARIES(idlc foundation idldocument)

IF(N_BUILD_FIRSTTIME)
	SET_TARGET_PROPERTIES(idlc PROPERTIES RELEASE_POSTFIX ".toolkit")
ENDIF()
