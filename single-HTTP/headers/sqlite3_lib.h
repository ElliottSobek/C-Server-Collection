#ifndef SQLITE3_LIB_H
#define SQLITE3_LIB_H

#include "types.h"
#include "resultset.h"

extern ResultSet sqlite_exec(const String restrict, ...);
extern void sqlite_load_fixture(const String restrict);
extern void sqlite_dumpdb(void);
extern void sqlite_dumptable(const String restrict);
extern void sqlite_load_exec(const String restrict);
extern String sqlite_get_version(void);

#endif /* End SQLITE3_LIB_H */
