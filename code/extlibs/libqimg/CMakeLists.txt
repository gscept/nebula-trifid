#-------------------------------------------------------------------------------
# libqimg
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(
    ${Nebula3_SOURCE_DIR}/code/foundation
    ${Nebula3_SOURCE_DIR}/code/extlibs/DevIL/DevIL/include
	${Nebula3_SOURCE_DIR}/code/extlibs/libqimg
)

FILE(GLOB SRC "*.cc")
FILE(GLOB HEAD "*.h")

PCH_QT_WRAP_CPP(MOC_SRC ${HEAD})
SOURCE_GROUP("libqimg" FILES ${SRC} ${HEAD})
LIST(APPEND SRCS ${SRC} ${HEAD} ${MOC_SRC})




SOURCE_GROUP(libqimg FILES ${SRCS})
ADD_LIBRARY(libqimg STATIC ${SRCS})
SET_TARGET_PROPERTIES(libqimg PROPERTIES FOLDER "extlibs")

TARGET_LINK_LIBRARIES(libqimg qtsupport)

ADD_DEFINITIONS(/DQT_STATICPLUGIN)

N_SET_COMPILER_DEFAULTS()