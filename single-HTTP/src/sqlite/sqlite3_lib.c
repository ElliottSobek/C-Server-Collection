#include <errno.h>
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

#include "globals.h"
#include "types.h"
#include "colors.h"
#include "sqlite3_lib.h"
#include "resultset.h"
#include "query.h"

#define ALL_TABLES -1
#define CONF_EXT_LEN 5

ResultSet sqlite_exec(const String restrict stmt, ...) {
    sqlite3 *db;
    sqlite3_stmt *sql_byte_code;
    int result_code = sqlite3_open(_db_path, &db);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "Database Error: Cannot open database: %s\n" RESET, sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    const Query restrict query = parse_stmt(stmt);

    if (!query) {
        fprintf(stderr, RED "Memory Error: %s\n" RESET, strerror(errno));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    result_code = sqlite3_prepare_v2(db, query->stmt, KBYTE_S * 2, &sql_byte_code, NULL);

    if (result_code != SQLITE_OK) {
        if (_verbose_flag)
            fprintf(stderr, YELLOW "SQL error: %s\n" RESET, sqlite3_errmsg(db));
        destroy_query(query);
        sqlite3_finalize(sql_byte_code);
        sqlite3_close(db);
        return NULL;
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
            default:
                puts("Unknown query arg specifier");
                break;
            }
        }
        va_end(args);
    }
    ResultSet rs;

    if (strncasecmp("SELECT", query->stmt, 6) == 0) {
        const int rows = sqlite3_column_count(sql_byte_code);

        rs = parse_query_result(rows, sql_byte_code);

        if (!rs)
            fprintf(stderr, RED "Memory Error: %s\n" RESET, strerror(errno));
    } else {
        sqlite3_step(sql_byte_code);
        const int affected = sqlite3_changes(db);
        rs = create_rs(1, (String[]) {"Rows_Affected", NULL}, NULL, affected);

        if (!rs)
            fprintf(stderr, RED "Memory Error: %s\n" RESET, strerror(errno));
        else if (_verbose_flag)
            printf("Rows affected: %d\n", affected);
    }
    destroy_query(query);
    sqlite3_finalize(sql_byte_code);
    sqlite3_close(db);

    return rs;
}

void sqlite_load_exec(const String restrict filepath) {
    sqlite3 *db;
    char buf[KBYTE_S], sql_buf[MBYTE_S] = {0};
    String err_msg;
    const String extension = strrchr(filepath, '.');

    if (!extension) {
        if (_verbose_flag)
            puts(RED "File Warning: -l option was not supplied a file" RESET);
        return;
    }

    if (strncmp(extension, ".sql", CONF_EXT_LEN) != 0) {
        if (_verbose_flag)
            puts(RED "File Warning: -l option takes an sql file as an argument\n" RESET);
        return;
    }
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
        fclose(fixture);
        sqlite3_close(db);
        return;
    }
    result_code = sqlite3_exec(db, sql_buf, NULL, NULL, &err_msg);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "SQL error: %s\n" RESET, err_msg);
        fclose(fixture);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return;
    }
    fclose(fixture);
    sqlite3_close(db);

    return;
}

void sqlite_dumpdb(void) {
    sqlite3 *d_db, *s_db;
    sqlite3_backup *backup;
    int result_code = sqlite3_open_v2(_db_path, &s_db, SQLITE_OPEN_READWRITE, NULL);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "Database Error: Cannot open source database: %s\n" RESET, sqlite3_errmsg(s_db));
        sqlite3_close(s_db);
        return;
    }
    result_code = sqlite3_open("database/copy.sqlite3", &d_db);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "Database Error: Cannot open destination database : %s\n" RESET, sqlite3_errmsg(d_db));
        sqlite3_close(s_db);
        sqlite3_close(d_db);
        return;
    }
    backup = sqlite3_backup_init(d_db, "main", s_db, "main");

    if (!backup) {
        fprintf(stderr, RED "Database Error: Cannot initalize database copy: %s\n" RESET, sqlite3_errmsg(d_db));
        sqlite3_backup_finish(backup);
        sqlite3_close(s_db);
        sqlite3_close(d_db);
        return;
    }
    result_code = sqlite3_backup_step(backup, ALL_TABLES);

    while (result_code == SQLITE_OK)
        result_code = sqlite3_backup_step(backup, ALL_TABLES);

    if (result_code != SQLITE_DONE) {
        fprintf(stderr, RED "Database Error: Cannot copy database: %s\n" RESET, sqlite3_errmsg(s_db));
        sqlite3_backup_finish(backup);
        sqlite3_close(s_db);
        sqlite3_close(d_db);
        return;
    }
    sqlite3_backup_finish(backup);
    sqlite3_close(s_db);
    sqlite3_close(d_db);

    return;
}

void sqlite_dumptable(const String restrict table) {
    sqlite3 *db;
    sqlite3_stmt *stmt_table, *stmt_data;
    char sql_stmt[4096] = {0};
    String data;
    int col_cnt, result_code = sqlite3_open(_db_path, &db);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "Database Error: Cannot open database: %s\n" RESET, sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    snprintf(sql_stmt, KBYTE_S, "SELECT sql, COUNT() FROM sqlite_master WHERE type = 'table' AND name = '%s'", table);
    result_code = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt_table, NULL);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "SQL error: %s\n" RESET, sqlite3_errmsg(db));
        return;
    }
    result_code = sqlite3_step(stmt_table);

    if (sqlite3_column_int(stmt_table, 1) == 0) {
        fprintf(stderr, RED "SQL error: table '%s' does not exist\n" RESET, table);
        return;
    }
    printf("PRAGMA foreign_keys=off;\nBEGIN TRANSACTION;\nDROP TABLE IF EXISTS %s;\n", table);
    data = (char*) sqlite3_column_text(stmt_table, 0);

    if (!data) {
        fprintf(stderr, RED "Database Error: Cannot open database: %s\n" RESET, sqlite3_errmsg(db));
        return;
    }

    printf("%s;\n", data);
    snprintf(sql_stmt, KBYTE_S, "SELECT * FROM %s;", table);
    result_code = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt_data, NULL);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "SQL error: %s\n" RESET, sqlite3_errmsg(db));
        return;
    }
    result_code = sqlite3_step(stmt_data);

    while (result_code == SQLITE_ROW) {
        snprintf(sql_stmt, KBYTE_S, "INSERT INTO \"%s\" VALUES (", table);
        col_cnt = sqlite3_column_count(stmt_data);

        for (int index = 0; index < col_cnt; index++) {
            if (index)
                strcat(sql_stmt, ",");
            data = (char*) sqlite3_column_text(stmt_data, index);

            if (data) {
                if (sqlite3_column_type(stmt_data, index) == SQLITE_TEXT) {
                    strcat(sql_stmt, "'");
                    strcat(sql_stmt, data);
                    strcat(sql_stmt, "'");
                } else
                    strcat(sql_stmt, data);
            } else
                strcat(sql_stmt, "NULL");
        }
        printf("%s);\n", sql_stmt);
        result_code = sqlite3_step(stmt_data);
    }
    result_code = sqlite3_step(stmt_table);

    if (stmt_table)
        sqlite3_finalize(stmt_table);
    result_code = sqlite3_prepare_v2(db, "SELECT sql FROM sqlite_master WHERE type = 'trigger';", -1, &stmt_table, NULL);

    if (result_code != SQLITE_OK) {
        fprintf(stderr, RED "SQL error: %s\n" RESET, sqlite3_errmsg(db));
        return;
    }
    result_code = sqlite3_step(stmt_table);

    while (result_code == SQLITE_ROW) {
        data = (char*) sqlite3_column_text(stmt_table, 0);

        if (!data) {
            fprintf(stderr, RED "Database Error: Cannot open database: %s\n" RESET, sqlite3_errmsg(db));
            return;
        }

        printf("%s;\n", data);
        result_code = sqlite3_step(stmt_table);
    }
    printf("COMMIT;\n");

    return;
}

String sqlite_get_version(void) {
    return "Sqlite3 Version " SQLITE_VERSION;
}
