#-------------------------------------------------------------------------------
# Lua
#-------------------------------------------------------------------------------
SET(extlibs_lua_SRCS
    lapi.cc
    lapi.h
    lauxlib.cc
    lauxlib.h
    lbaselib.cc
    lcode.cc
    lcode.h
    ldblib.cc
    ldebug.cc
    ldebug.h
    ldo.cc
    ldo.h
    ldump.cc
    lfunc.cc
    lfunc.h
    lgc.cc
    lgc.h
    linit.cc
    llex.cc
    llex.h
    llimits.h
    lmathlib.cc
    lmem.cc
    lmem.h
    lobject.cc
    lobject.h
    lopcodes.cc
    lopcodes.h
    lparser.cc
    lparser.h
    lstate.cc
    lstate.h
    lstring.cc
    lstring.h
    lstrlib.cc
    ltable.cc
    ltable.h
    ltablib.cc
    ltm.cc
    ltm.h
    lua.h
    luaconf.h
    lualib.h
    lundump.cc
    lundump.h
    lvm.cc
    lvm.h
    lzio.cc
    lzio.h
    print.cc
	liolib.cc
	loadlib.cc
	loslib.cc	
)
SOURCE_GROUP(lua FILES ${extlibs_lua_SRCS})

ADD_LIBRARY(lua STATIC ${extlibs_lua_SRCS})
  SET_TARGET_PROPERTIES(lua PROPERTIES FOLDER "extlibs")

N_SET_COMPILER_DEFAULTS()
IF(MSVC)
    ADD_DEFINITIONS(/D_CRT_SECURE_NO_WARNINGS)
ENDIF()
ADD_DEPENDENCIES(lua foundation)
