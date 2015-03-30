#-------------------------------------------------------------------------------
# Sqlite3
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(
    ${Nebula3_SOURCE_DIR}/foundation/
    ${Nebula3_SOURCE_DIR}/extlibs/
)
SET(extlibs_sqlite_SRCS
sqlite3.h
sqlite3.c

)
SOURCE_GROUP(sqlite FILES ${extlibs_sqlite_SRCS})
ADD_LIBRARY(sqlite STATIC ${extlibs_sqlite_SRCS})
SET_TARGET_PROPERTIES(sqlite PROPERTIES FOLDER "extlibs")

IF(UNIX)
SET(CMAKE_MODULE_LINKER_FLAGS "-ldl")
ENDIF()

N_SET_COMPILER_DEFAULTS()

ADD_DEPENDENCIES(sqlite foundation libsqlite3)

