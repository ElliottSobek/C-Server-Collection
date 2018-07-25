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
    char *prepare_stmt = strdup(stmt);
    char result[1024] = "";
    char char_holder[2] = {'\0', '\0'};
    const size_t prepare_stmt_len = strlen(stmt);

    for (unsigned int i = 0; i < prepare_stmt_len; i++) {
        if (prepare_stmt[i] == '%') {
            strcat(result, "?");
            i++;
            continue;
        }
        char_holder[0] = prepare_stmt[i];

        strcat(result, char_holder);
    }

    printf("%s\n", result);

    return NULL;
}

// char *reduce(const char *const snipet) {
//     char snip[255];
//     strcpy(snip, snipet);
//     printf("%s\n", snip);
//     // const char *token = strtok(snip, "%");
//     // char *result;
//     // return strdup(result);
//     return NULL;
// }

// char *prepare(const char *const stmt) {
//     char stmt2[1024];
//     strcpy(stmt2, stmt);
//     char *stmt2_copy = stmt2;
//     char prepare_stmt[1024] = "";
//     const char *token = strtok_r(stmt2_copy, " ", &stmt2_copy);
//     const char *token2;
//     // unsigned int index;
//     // char tmp1[255] = "";

//     strcat(prepare_stmt, token);
//     printf("%s", token);
//     token = strtok_r(NULL, " ", &stmt2_copy);

//     while (token) {
//         token2 = strchr(token, '%');

//         if (token2) {
//             // strcat(prepare_stmt, " ");
//             printf("\nThis is token before: %s\n", token);
//             reduce(token);
//             printf("\nThis is token after: %s\n", token);
//             ;
//         }
//         printf(" %s", token);
//         strcat(prepare_stmt, " ");
//         strcat(prepare_stmt, token);
//         token = strtok_r(NULL,  "", &stmt2_copy);
//     }

//     return NULL;
// }

// char *prepare(const char *const stmt) {
//     const char *search_str = strchr(stmt, '%');
//     char parameter_stmt[KBYTE_S] = "";
//     unsigned int prev = 0, index;
//     bool offset = false;

//     if (!search_str)
//         return NULL;

//     while (search_str) {
//         index = search_str - stmt;

//         printf("Prev going in: %d\n", prev);
//         for (unsigned int i = 0; i <= index - prev; i++) {

//             if (stmt[prev] == '%') {
//                 prev++;
//                 offset = true;
//             }
//             if (offset) {
//                 printf("-%c-\n", parameter_stmt[prev + i - 1]);
//                 printf("-%c-\n", parameter_stmt[prev + i]);
//                 printf("-%c-\n", parameter_stmt[prev + i + 1]);
//                 printf("-%c-\n", stmt[prev + i]);
//                 printf("-%c-\n", stmt[prev + i + 1]);
//                 printf("-%c-\n", stmt[prev + i + 2]);
//                 printf("This is parameter_stmt char in offset: -%c- and index: %d\n", parameter_stmt[prev + i], prev + i);
//                 printf("This is stmt char in offset: -%c- and index: %d\n", stmt[prev + i + 1], prev + i + 2);
//                 // parameter_stmt[prev + i] = stmt[prev + i + 2];
//                 i++;
//                 offset = false;
//             } else {
//                 printf("This is parameter_stmt char: -%c- and index: %d\n", parameter_stmt[prev + i], prev + i);
//                 printf("This is stmt char: -%c- and index: %d\n", stmt[prev + i], prev + i);
//                 parameter_stmt[prev + i] = stmt[prev + i];
//             }
//         }

//         parameter_stmt[index] = '?';
//         parameter_stmt[index + 1] = '\0';
//         search_str = strchr(++search_str, '%');
//         prev = index;
//     }
//     strncat(parameter_stmt, ";", 1);
//     printf("This is parameter_stmt: %s\n", parameter_stmt);
//     return strndupa(parameter_stmt, KBYTE_S);
// }

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
