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

#include "../../debug.h"

#define ALL_TABLES -1

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
    sqlite3 *db;
    char buf[KBYTE_S], sql_buf[MBYTE_S] = {0};
    String err_msg;
    FILE *fixture = fopen(filepath, "r");

    if (!fixture) {
        fprintf(stderr, RED "%s\n" RESET, strerror(errno));
        return;
    }

    while (fgets(buf, KBYTE_S, fixture))
        strncat(sql_buf, buf, KBYTE_S);
    int result_code = sqlite3_open(_db_path, &db);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "Database Error: Cannot open database: %s\n" RESET, sqlite3_errmsg(db));
        return;
    }
    result_code = sqlite3_exec(db, sql_buf, NULL, NULL, &err_msg);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "SQL error: %s\n" RESET, err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return;
    }
    sqlite3_close(db);

    return;
}

void sqlite_dumpdb(void) {
    sqlite3 *d_db, *s_db;
    sqlite3_backup *backup;
    int result_code = sqlite3_open(_db_path, &s_db);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "Database Error: Cannot open source database: %s\n" RESET, sqlite3_errmsg(s_db));
        return;
    }
    result_code = sqlite3_open("database/copy.sqlite3", &d_db);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "Database Error: Cannot open destination database : %s\n" RESET, sqlite3_errmsg(d_db));
        return;
    }
    backup = sqlite3_backup_init(d_db, "main", s_db, "main");

    if (!backup) {
        fprintf(stderr, RED "Database Error: Cannot initalize database copy: %s\n" RESET, sqlite3_errmsg(d_db));
        return;
    }
    result_code = sqlite3_backup_step(backup, ALL_TABLES);

    while (result_code == SQLITE_OK)
        result_code = sqlite3_backup_step(backup, ALL_TABLES);

    if (result_code != SQLITE_DONE) {
        fprintf(stderr, RED "Database Error: Cannot copy database: %s\n" RESET, sqlite3_errmsg(s_db));
        return;
    }
    sqlite3_backup_finish(backup);
    sqlite3_close(s_db);

    return;
}

void sqlite_dumptable(const String restrict table) {
    sqlite3 *db;
    sqlite3_stmt *stmt_table, *stmt_data;
    const char *table_name, *data;
    int col_cnt = 0, ret = 0;
    char cmd[4096] = {0};

    ret = sqlite3_open(_db_path, &db);

    if (ret != SQLITE_OK) {
        fprintf(stderr, RED "Database Error: Cannot open database: %s\n" RESET, sqlite3_errmsg(db));
        return;
    }

    ret = sqlite3_prepare_v2(db, "SELECT sql, tbl_name FROM sqlite_master WHERE type = 'table'", -1, &stmt_table, NULL);

    if (ret != SQLITE_OK)
        return;

    printf("PRAGMA foreign_keys=off;\nBEGIN TRANSACTION;\n");

    ret = sqlite3_step(stmt_table);

    while (ret == SQLITE_ROW) {
        data = (char*) sqlite3_column_text(stmt_table, 0);
        table_name = (char*) sqlite3_column_text(stmt_table, 1);

        if (!data || !table_name)
            return;

        printf("%s;\n", data);
        sprintf(cmd, "SELECT * FROM %s;", table_name);

        ret = sqlite3_prepare_v2(db, cmd, -1, &stmt_data, NULL);

        if (ret != SQLITE_OK)
            return;

        ret = sqlite3_step(stmt_data);

        while (ret == SQLITE_ROW) {
            sprintf(cmd, "INSERT INTO \"%s\" VALUES (", table_name);
            col_cnt = sqlite3_column_count(stmt_data);

            for (int index = 0; index < col_cnt; index++) {
                if (index)
                    strcat(cmd, ",");
                data = (char*) sqlite3_column_text(stmt_data, index);

                if (data) {
                    if (sqlite3_column_type(stmt_data, index) == SQLITE_TEXT) {
                        strcat(cmd, "'");
                        strcat(cmd, data);
                        strcat(cmd, "'");
                    } else
                        strcat(cmd, data);
                } else
                    strcat(cmd, "NULL");
            }
            printf("%s);\n", cmd);
            ret = sqlite3_step(stmt_data);
        }
        ret = sqlite3_step(stmt_table);
    }

    if (stmt_table)
        sqlite3_finalize(stmt_table);

    ret = sqlite3_prepare_v2(db, "SELECT sql FROM sqlite_master WHERE type = 'trigger';", -1, &stmt_table, NULL);

    if (ret != SQLITE_OK)
        return;

    ret = sqlite3_step(stmt_table);

    while (ret == SQLITE_ROW) {
        data = (char*) sqlite3_column_text(stmt_table, 0);

        if (!data)
            return;

        printf("%s;\n", data);

        ret = sqlite3_step(stmt_table);
    }

    printf("COMMIT;\n");

    return;
}

String sqlite_get_version(void) {
    return "Sqlite3 Version " SQLITE_VERSION;
}
