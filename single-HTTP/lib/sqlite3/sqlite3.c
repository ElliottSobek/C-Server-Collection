#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sqlite3.h>

#include "../../globals.h"
#include "../types/types.h"
#include "../colors/colors.h"

static int result_set(void *unused, int col_num, char **row_value, char **col_name) {
    if (!verbose_flag)
        return -1;

    printf("| %s |", row_value[0] ? row_value[0]: "NULL");

    for (int i = 1; i < col_num; i++)
        printf(" %s |", row_value[i] ? row_value[i]: "NULL");
    printf("\n");
    return 0;
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

int select_debug(const String const restrict stmt) {
    sqlite3 *db;
    sqlite3_stmt *sql_byte_code;
    int result_code = sqlite3_open("database/db.sqlite3", &db);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "Database Error: Cannot open database: %s\n" RESET, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    result_code = sqlite3_prepare_v2(db, stmt, KBYTE_S * 2, &sql_byte_code, NULL);

    if (result_code != SQLITE_OK) {
        if (verbose_flag)
            fprintf(stderr, YELLOW "SQL error: %s\n" RESET, sqlite3_errmsg(db));
        sqlite3_finalize(sql_byte_code);
        sqlite3_close(db);
        return -1;
    }
    const int rows = sqlite3_column_count(sql_byte_code);

    print_headers(rows, sql_byte_code);
    print_rows(rows, sql_byte_code);
    sqlite3_finalize(sql_byte_code);
    sqlite3_close(db);

    return 0;
}

char *prepare(const char *const stmt) {
    const size_t prepare_stmt_len = strlen(stmt);
    char prepare_stmt[KBYTE_S * 2];
    char result[KBYTE_S] = "";

    strncpy(prepare_stmt, stmt, prepare_stmt_len + NT_LEN);

    for (unsigned int i = 0, j = 0; i < prepare_stmt_len; i++, j++) {
        if (prepare_stmt[i] == '%') {
            result[j] = '?';
            i++;
        } else
            result[j] = prepare_stmt[i];
    }

    return NULL;
}

// NOTE: char &sql = "SELECT * FROM Cars WHERE Id = @id;";
// char *sql = "SELECT * FRMO Cars WHERE Id = ?;";
// rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
//  if (rc == SQLITE_OK)
//      sqlite3_bind_int(res, 1, 3);
//  else
//      fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
int sqlite_exec(const String const restrict stmt) {
    sqlite3 *db;
    String err_msg;
    int result_code = sqlite3_open("database/db.sqlite3", &db);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "Database Error: Cannot open database: %s\n" RESET, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    if (strncasecmp("SELECT", stmt, 6) == 0)
        result_code = sqlite3_exec(db, stmt, result_set, NULL, &err_msg);
    else {
        result_code = sqlite3_exec(db, stmt, NULL, NULL, &err_msg);

        if (verbose_flag)
            printf("Rows affected: %d\n", sqlite3_changes(db));
    }

    if (result_code != SQLITE_OK) {
        if (verbose_flag)
            fprintf(stderr, YELLOW "SQL error: %s\n" RESET, err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }
    sqlite3_close(db);

    return 0;
}
