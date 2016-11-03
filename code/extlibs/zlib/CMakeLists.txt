#-------------------------------------------------------------------------------
# ZLib
#-------------------------------------------------------------------------------

SET(extlibs_zlib_SRCS
    adler32.c
    compress.c
    crc32.c
    crc32.h
    crypt.h
    deflate.c
    deflate.h
    gzio.c
    infback.c
    inffast.c
    inffast.h
    inffixed.h
    inflate.c
    inflate.h
    inftrees.c
    inftrees.h
    ioapi.h
    ionebula3.cc
    ionebula3.h
    trees.c
    trees.h
    uncompr.c
    unzip.c
    unzip.h
    zconf.h
    zconf.in.h
    zlib.h
    zutil.c
    zutil.h
)
SOURCE_GROUP(zlib FILES ${extlibs_zlib_SRCS})

ADD_LIBRARY(zlib STATIC ${extlibs_zlib_SRCS})
TARGET_INCLUDE_DIRECTORIES(zlib PUBLIC ${Nebula3_SOURCE_DIR}/code/extlibs ${Nebula3_SOURCE_DIR}/code/foundation)
SET_TARGET_PROPERTIES(zlib PROPERTIES FOLDER "extlibs")

N_SET_COMPILER_DEFAULTS(zlib)
IF(MSVC)
    ADD_DEFINITIONS(/D_CRT_SECURE_NO_WARNINGS)
ENDIF()
