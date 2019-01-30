#ifndef QUERY_H
#define QUERY_H

#include <stdbool.h>

#include "types.h"

typedef struct {
    String stmt, specifiers;
    bool is_parameterized;
    size_t specifiers_len;
} quert_t;

typedef quert_t *Query;

extern Query parse_stmt(const String);
extern void destroy_query(Query);

#endif /* End QUERY_H */
