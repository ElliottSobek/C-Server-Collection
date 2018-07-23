#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sqlite3.h>

#include "../types/types.h"
#include "../colors/colors.h"

#define KBYTE_S 1024

static int result_set(void *unused, const int col_num, char **row_value, char **col_name) {
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

int select2(const String const restrict stmt) {
    sqlite3 *db;
    sqlite3_stmt *sql_byte_code;
    int result_code = sqlite3_open("database/db.sqlite3", &db);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "Database Error: Cannot open database: %s\n" RESET, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
    result_code = sqlite3_prepare_v2(db, stmt, KBYTE_S * 2, &sql_byte_code, NULL);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, YELLOW "SQL error: %s\n" RESET, sqlite3_errmsg(db));
        sqlite3_finalize(sql_byte_code);
        sqlite3_close(db);
        return -1;
    }
    const int rows = sqlite3_column_count(sql_byte_code);

    print_headers(rows, sql_byte_code);
    print_rows(rows, sql_byte_code);
    sqlite3_finalize(sql_byte_code);
    const int rows_affected = sqlite3_changes(db);

    sqlite3_close(db);

    return rows_affected;
}

// printf("This is sqlite3_data_count: %d\n", sqlite3_data_count(res));
// printf("This is sqlite3_column_count: %d\n", rows);
int select(const String const restrict stmt) {
    sqlite3 *db;
    String err_msg;
    int result_code = sqlite3_open("database/db.sqlite3", &db);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "Database Error: Cannot open database: %s\n" RESET, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    if (strncasecmp("SELECT", stmt, 6) == 0)
        result_code = sqlite3_exec(db, stmt, result_set, NULL, &err_msg);
    else
        result_code = sqlite3_exec(db, stmt, NULL, NULL, &err_msg);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, YELLOW "SQL error: %s\n" RESET, err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }
    const int rows_affected = sqlite3_changes(db);

    sqlite3_close(db);

    return rows_affected;
}
