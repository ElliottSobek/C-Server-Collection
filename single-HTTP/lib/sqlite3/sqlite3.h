#ifndef SQLITE3_H
#define SQLITE3_H

#include "../types/types.h"

extern int sqlite_exec(const String restrict, ...);
extern void sqlite_load_fixture(const String restrict);
extern void sqlite_dump_db(const String restrict);
extern void sqlite_load_exec(const String restrict);

#endif /* End SQLITE3_H */
