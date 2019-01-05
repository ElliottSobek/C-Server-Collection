#ifndef SQLITE3_H
#define SQLITE3_H

#include "types.h"

extern int sqlite_exec(const String restrict, ...);
extern void sqlite_load_fixture(const String restrict);
extern void sqlite_dumpdb(void);
extern void sqlite_dumptable(const String restrict);
extern void sqlite_load_exec(const String restrict);
extern String sqlite_get_version(void);

#endif /* End SQLITE3_H */
