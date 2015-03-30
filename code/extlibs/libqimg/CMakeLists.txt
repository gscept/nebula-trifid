#-------------------------------------------------------------------------------
# libqimg
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(
    ${Nebula3_SOURCE_DIR}/code/foundation
    ${Nebula3_SOURCE_DIR}/code/extlibs/DevIL/DevIL/include
	${Nebula3_SOURCE_DIR}/code/extlibs/libqimg
)

FIND_PACKAGE(Qt4 REQUIRED)

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")
QT4_WRAP_CPP(MOC_SRC ${HEAD})
SOURCE_GROUP("libqimg" FILES ${SRC} ${HEAD})
LIST(APPEND SRCS ${SRC} ${HEAD} ${MOC_SRC})

#FILE(GLOB SRC "ddsutil/*.cc")
#FILE(GLOB HEAD "ddsutil/*.h")
#SOURCE_GROUP("ddsutil" FILES ${SRC} ${HEAD})
#LIST(APPEND SRCS ${SRC} ${HEAD})

#FILE(GLOB SRC "tgautil/*.cc")
#FILE(GLOB HEAD "tgautil/*.h")
#SOURCE_GROUP("tgautil" FILES ${SRC} ${HEAD})
#LIST(APPEND SRCS ${SRC} ${HEAD})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

SOURCE_GROUP(libqimg FILES ${SRCS})
ADD_LIBRARY(libqimg STATIC ${SRCS})
SET_TARGET_PROPERTIES(libqimg PROPERTIES FOLDER "extlibs")

ADD_DEFINITIONS(/DQT_STATICPLUGIN)

N_SET_COMPILER_DEFAULTS()