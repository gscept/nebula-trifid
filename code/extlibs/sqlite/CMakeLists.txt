#-------------------------------------------------------------------------------
# Sqlite
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(
    ${Nebula3_SOURCE_DIR}/foundation/
    ${Nebula3_SOURCE_DIR}/extlibs/
)
SET(extlibs_sqlite_SRCS
sqlite3.h
sqlite3ext.h
sqliteInt.h
sqlite_alter.c
sqlite_analyze.c
sqlite_attach.c
sqlite_auth.c
sqlite_btree.c
sqlite_btree.h
sqlite_build.c
sqlite_callback.c
sqlite_complete.c
sqlite_date.c
sqlite_delete.c
sqlite_expr.c
sqlite_func.c
sqlite_hash.c
sqlite_hash.h
sqlite_insert.c
sqlite_keywordhash.h
sqlite_legacy.c
sqlite_loadext.c
sqlite_main.c
sqlite_opcodes.c
sqlite_opcodes.h
sqlite_os.c
sqlite_os.h
sqlite_os_common.h
sqlite_os_os2.c
sqlite_os_other.cc
sqlite_os_unix.c
sqlite_os_win.c
sqlite_pager.c
sqlite_pager.h
sqlite_parse.c
sqlite_parse.h
sqlite_pragma.c
sqlite_prepare.c
sqlite_printf.c
sqlite_random.c
sqlite_select.c
sqlite_table.c
sqlite_tokenize.c
sqlite_trigger.c
sqlite_update.c
sqlite_utf.c
sqlite_util.c
sqlite_vacuum.c
sqlite_vdbe.c
sqlite_vdbe.h
sqlite_vdbeapi.c
sqlite_vdbeaux.c
sqlite_vdbefifo.c
sqlite_vdbeInt.h
sqlite_vdbemem.c
sqlite_vtab.c
sqlite_where.c

)
SOURCE_GROUP(sqlite FILES ${extlibs_sqlite_SRCS})
ADD_LIBRARY(sqlite STATIC ${extlibs_sqlite_SRCS})
SET_TARGET_PROPERTIES(sqlite PROPERTIES FOLDER "extlibs")

N_SET_COMPILER_DEFAULTS()
ADD_DEPENDENCIES(sqlite foundation)
