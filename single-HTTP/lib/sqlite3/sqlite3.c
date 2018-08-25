#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <strings.h>
#include <sqlite3.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../../globals.h"
#include "../types/types.h"
#include "../colors/colors.h"

typedef struct query_s {
    String stmt, specifiers;
    bool is_parameterized;
    size_t specifiers_len;
} quert_t;

typedef quert_t *Query;

static Query parse_stmt(const String restrict stmt) {
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

static void destroy_query(Query query) {
    free(query->stmt);
    query->stmt = NULL;

    free(query->specifiers);
    query->specifiers = NULL;

    free(query);
    query = NULL;
    return;
}

static void print_headers(const int rows, sqlite3_stmt *sql_byte_code) {
    sqlite3_step(sql_byte_code);
    printf("| %s |", sqlite3_column_name(sql_byte_code, 0));

    for (int i = 1; i < rows; i++)
        printf(" %s |", sqlite3_column_name(sql_byte_code, i));
    printf("\n");
}

static void print_rows(const int rows, sqlite3_stmt *sql_byte_code) {
    int result;
    const char *row_value;

    do {
        row_value = (char*) sqlite3_column_text(sql_byte_code, 0);

        printf("| %s |", row_value ? row_value: "NULL");

        for (int i = 1; i < rows; i++) {
            row_value = (char*) sqlite3_column_text(sql_byte_code, i);

            printf(" %s |", row_value ? row_value: "NULL");
        }
        printf("\n");
        result = sqlite3_step(sql_byte_code);

    } while(result == SQLITE_ROW);
}

int sqlite_exec(const String restrict stmt, ...) {
    sqlite3 *db;
    sqlite3_stmt *sql_byte_code;
    int result_code = sqlite3_open(_db_path, &db);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "Database Error: Cannot open database: %s\n" RESET, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    const Query restrict query = parse_stmt(stmt);
    result_code = sqlite3_prepare_v2(db, query->stmt, KBYTE_S * 2, &sql_byte_code, NULL);

    if (result_code != SQLITE_OK) {
        if (verbose_flag)
            fprintf(stderr, YELLOW "SQL error: %s\n" RESET, sqlite3_errmsg(db));
        sqlite3_finalize(sql_byte_code);
        sqlite3_close(db);
        return -1;
    }

    if (query->is_parameterized) {
        char c;
        va_list args;
        struct stat file;
        String string;

        va_start(args, stmt);

        for (unsigned short i = 0; i < query->specifiers_len; i++) {
            c = query->specifiers[i];

            switch (c) {
            case 'd':
                sqlite3_bind_int(sql_byte_code, i + 1, va_arg(args, int));
                continue;
            case 's':
                string = va_arg(args, char*);

                sqlite3_bind_text(sql_byte_code, i + 1, string, strnlen(string, PATH_MAX), SQLITE_STATIC);
                continue;
            case 'f':
                sqlite3_bind_double(sql_byte_code, i + 1, va_arg(args, double));
                continue;
            case 'b':
                string = va_arg(args, char*);

                stat(string, &file);
                sqlite3_bind_blob(sql_byte_code, i + 1, string, file.st_size, SQLITE_STATIC);
                continue;
            }
        }
        va_end(args);
    }

    if (strncasecmp("SELECT", query->stmt, 6) == 0) {
        const int rows = sqlite3_column_count(sql_byte_code);

        print_headers(rows, sql_byte_code);
        print_rows(rows, sql_byte_code);
    } else {
        sqlite3_step(sql_byte_code);

        if (verbose_flag)
            printf("Rows affected: %d\n", sqlite3_changes(db));
    }
    destroy_query(query);
    sqlite3_finalize(sql_byte_code);
    sqlite3_close(db);

    return 0;
}


void sqlite_load_exec(const String restrict filepath) {
    FILE *fixture = fopen(filepath, "r");
    char buf[KBYTE_S], sql_buf[MBYTE_S] = "";

    while (fgets(buf, KBYTE_S, fixture))
        strncat(sql_buf, buf, KBYTE_S);

    sqlite_exec(sql_buf);
}

void sqlite_dumpdata(const String restrict table) {
    if (!table)
        puts("Database");
    else
        puts("Specific table");
    return;
}

String sqlite_get_version(void) {
    return SQLITE_VERSION;
}
