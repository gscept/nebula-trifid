#-------------------------------------------------------------------------------
# db
#-------------------------------------------------------------------------------

set(SOURCES_files_db__sqlite3
	sqlite3/sqlite3command.cc
	sqlite3/sqlite3command.h
	sqlite3/sqlite3database.cc
	sqlite3/sqlite3database.h
	sqlite3/sqlite3dataset.cc
	sqlite3/sqlite3dataset.h
	sqlite3/sqlite3factory.cc
	sqlite3/sqlite3factory.h
	sqlite3/sqlite3filterset.cc
	sqlite3/sqlite3filterset.h
	sqlite3/sqlite3table.cc
	sqlite3/sqlite3table.h
)
source_group("db\\sqlite3" FILES ${SOURCES_files_db__sqlite3})

set(SOURCES_db__sqlite3
	${SOURCES_files_db__sqlite3}
)

set(SOURCES_files_db
	column.cc
	column.h
	command.cc
	command.h
	database.cc
	database.h
	dataset.cc
	dataset.h
	dbfactory.cc
	dbfactory.h
	dbserver.cc
	dbserver.h
	filterset.cc
	filterset.h
	reader.cc
	reader.h
	relation.cc
	relation.h
	table.cc
	table.h
	valuetable.cc
	valuetable.h
	writer.cc
writer.h
)
source_group("db" FILES ${SOURCES_files_db})

set(SOURCES_db
${SOURCES_db__sqlite3}
${SOURCES_files_db}
)

set(SOURCES_files
../stdneb.h
../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
set(SOURCES_SRCS
${SOURCES_db}
${SOURCES_files}
)

add_library(db STATIC ${SOURCES_SRCS} )
TARGET_LINK_LIBRARIES(db PUBLIC foundation sqlite)
TARGET_INCLUDE_DIRECTORIES(db PUBLIC ${Nebula3_SOURCE_DIR}/code/addons)
SET_TARGET_PROPERTIES(db PROPERTIES FOLDER "N3SDK/addons")
N_SET_COMPILER_DEFAULTS(db)
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(db stdneb.h stdneb.cc ../)
ENDIF()

