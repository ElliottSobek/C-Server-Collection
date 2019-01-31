#ifndef SQLITE3_LIB_H
#define SQLITE3_LIB_H

#include "types.h"
#include "resultset.h"

ResultSet sqlite_exec(const String restrict, ...);
void sqlite_load_fixture(const String restrict);
void sqlite_dumpdb(void);
void sqlite_dumptable(const String restrict);
void sqlite_load_exec(const String restrict);
String sqlite_get_version(void);

#endif /* End SQLITE3_LIB_H */
