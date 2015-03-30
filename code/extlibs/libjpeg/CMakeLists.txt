#-------------------------------------------------------------------------------
# libjpeg
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/extlibs/libjpg
					)
FILE(GLOB SRC "*.c")
FILE(GLOB HEAD "*.h")
LIST(APPEND extlibs_libjpg_SRCS ${SRC} ${HEAD})

SOURCE_GROUP(libjpeg FILES ${extlibs_libjpg_SRCS})

ADD_LIBRARY(libjpeg STATIC ${extlibs_libjpg_SRCS})
SET_TARGET_PROPERTIES(libjpeg PROPERTIES FOLDER "extlibs")

N_SET_COMPILER_DEFAULTS()
SET(CMAKE_C_FLAGS "")
