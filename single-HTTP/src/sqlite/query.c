#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "types.h"
#include "query.h"

static Query create_query(const String stmt, const String specifiers, const bool is_param, const size_t specifiers_len) {
    Query query = (Query) malloc(sizeof(query_t));

    if (!query)
        return NULL;

    query->stmt = (String) calloc(2048, sizeof(char));

    if (!query->stmt) {
        free(query);
        query = NULL;

        return NULL;
    }
    strncpy(query->stmt, stmt, KBYTE_S * 2);
    query->specifiers = (String) calloc(64, sizeof(char));

    if (!query->specifiers) {
        free(query->stmt);
        query->stmt = NULL;

        free(query);
        query = NULL;

        return NULL;
    }
    strncpy(query->specifiers, specifiers, 63);

    query->is_parameterized = is_param;
    query->specifiers_len = specifiers_len;

    return query;
}

Query parse_stmt(const String restrict stmt) {
    const size_t prepare_stmt_len = strnlen(stmt, KBYTE_S * 2);
    char prepare_stmt[(KBYTE_S * 2) + NT_LEN], result[(KBYTE_S * 2) + NT_LEN] = "", specifiers[63 + NT_LEN] = "";

    strncpy(prepare_stmt, stmt, prepare_stmt_len);

    for (unsigned int i = 0, j = 0, k = 0; i < prepare_stmt_len; i++, j++) {
        if (prepare_stmt[i] == '%') {
            result[j] = '?';
            specifiers[k] = prepare_stmt[i + 1];
            i++;
            k++;
        } else
            result[j] = prepare_stmt[i];
    }

    if (specifiers[0])
        return create_query(result, specifiers, true, strnlen(specifiers, 63));
    return create_query(result, specifiers, false, 0);
}

void destroy_query(Query query) {
    free(query->stmt);
    query->stmt = NULL;

    free(query->specifiers);
    query->specifiers = NULL;

    free(query);
    query = NULL;
    return;
}
