#ifndef QUERY_H
#define QUERY_H

#include <stdbool.h>

#include "types.h"

typedef struct {
    String stmt, specifiers;
    bool is_parameterized;
    size_t specifiers_len;
} query_t;

typedef query_t *Query;

Query parse_stmt(const String);
void destroy_query(Query);

#endif /* End QUERY_H */
