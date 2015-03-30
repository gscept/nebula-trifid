#-------------------------------------------------------------------------------
# libpng
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/extlibs/libpng
					${Nebula3_SOURCE_DIR}/code/extlibs/zlib
					)
FILE(GLOB SRC "*.c")
FILE(GLOB HEAD "*.h")
LIST(APPEND extlibs_libpng_SRCS ${SRC} ${HEAD})

SOURCE_GROUP(libpng FILES ${extlibs_libpng_SRCS})

ADD_LIBRARY(libpng STATIC ${extlibs_libpng_SRCS})
SET_TARGET_PROPERTIES(libpng PROPERTIES FOLDER "extlibs")

N_SET_COMPILER_DEFAULTS()
SET(CMAKE_C_FLAGS "")
