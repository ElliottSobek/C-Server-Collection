#ifndef RESULTSET_H
#define RESULTSET_H

#include <sqlite3.h>

#include "s_linked_list.h"

typedef struct {
    String *column_names; // E.g. {"One", "Two", "Three"}
    S_Ll query_result; // E.g. [("One", "Two", "Three"), ("One", "Two", "Three"), ("One", "Two", "Three")]
    unsigned short column_count;
    unsigned long result_amount;
} resultset_t;

typedef resultset_t *ResultSet;

extern ResultSet create_rs(const int, String*, S_Ll, unsigned long);
extern ResultSet parse_query_result(const int, sqlite3_stmt*);
extern void sqlite_destroy_rs(ResultSet);
extern void sqlite_print_rs(ResultSet);

#endif /* End RESULTSET_H */
