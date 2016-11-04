#-------------------------------------------------------------------------------
# Sqlite3
#-------------------------------------------------------------------------------

SET(extlibs_sqlite_SRCS
	sqlite3.h
	sqlite3.c
)
SOURCE_GROUP(sqlite FILES ${extlibs_sqlite_SRCS})
IF(CMAKE_COMPILER_IS_GNUCC)
add_compile_options("-fno-fast-math")
ENDIF()
ADD_LIBRARY(sqlite STATIC ${extlibs_sqlite_SRCS})
TARGET_LINK_LIBRARIES(sqlite PUBLIC foundation)
TARGET_INCLUDE_DIRECTORIES(sqlite PUBLIC ${Nebula3_SOURCE_DIR}/code/extlibs/sqlite3)
SET_TARGET_PROPERTIES(sqlite PROPERTIES FOLDER "extlibs")

IF(UNIX)
SET(CMAKE_MODULE_LINKER_FLAGS "-ldl")
ENDIF()

N_SET_COMPILER_DEFAULTS(sqlite)
