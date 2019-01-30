#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "types.h"
#include "query.h"

// TODO: create Query

// Rename?
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

    Query query = (Query) malloc(sizeof(quert_t));

    if (!query)
        exit(EXIT_FAILURE);

    // const size_t result_len = strnlen(result, KBYTE_S * 2);

    query->stmt = (String) calloc(2048, sizeof(char));

    if (!query->stmt)
        exit(EXIT_FAILURE);
    strncpy(query->stmt, result, KBYTE_S * 2);
    // (KBYTE_S * 2) + NT_LEN
    query->specifiers = (String) calloc(64, sizeof(char));

    if (!query->specifiers)
        exit(EXIT_FAILURE);

    if (specifiers[0] != '\0') {
        query->is_parameterized = true;
        query->specifiers_len = strnlen(specifiers, 63);
        strncpy(query->specifiers, specifiers, 63);
        return query;
    }
    query->is_parameterized = false;
    query->specifiers_len = 0;

    return query;
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
