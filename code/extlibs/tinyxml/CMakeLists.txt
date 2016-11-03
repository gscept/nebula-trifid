#-------------------------------------------------------------------------------
# TinyXML
#-------------------------------------------------------------------------------

SET(extlibs_tinyxml_src
    tinystr.cc
    tinystr.h
    tinyxml.cc
    tinyxml.h
    tinyxmlerror.cc
    tinyxmlparser.cc
)
SET(extlibs_tinyxml_pch_src
    ../stdneb.cc
    ../stdneb.h
)
SET(extlibs_tinyxml_SRCS
    ${extlibs_tinyxml_src}
    ${extlibs_tinyxml_pch_src}
)
SOURCE_GROUP(tinyxml FILES ${extlibs_tinyxml_src})
SOURCE_GROUP(precompiled FILES ${extlibs_tinyxml_pch_src})

ADD_LIBRARY(tinyxml STATIC ${extlibs_tinyxml_SRCS})
TARGET_INCLUDE_DIRECTORIES(tinyxml PUBLIC ${Nebula3_SOURCE_DIR}/code/extlibs ${Nebula3_SOURCE_DIR}/code/foundation)
SET_TARGET_PROPERTIES(tinyxml PROPERTIES FOLDER "extlibs")

N_SET_COMPILER_DEFAULTS(tinyxml)
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(tinyxml stdneb.h stdneb.cc ../)
ENDIF()
