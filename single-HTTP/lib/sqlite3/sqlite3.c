#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "../types/types.h"
#include "../colors/colors.h"

int callback(void *unused, const int col_num, char **row_value, char **col_name) {
    if (col_num < 1)
        return -1;

    printf("| %s |", row_value[0] ? row_value[0]: "NULL");

    for (int i = 1; i < col_num; i++)
        printf(" %s |", row_value[i] ? row_value[i]: "NULL");
    printf("\n");
    return 0;
}

int select(const String const restrict stmt) {
    sqlite3 *db;
    String err_msg;
    int con = sqlite3_open("database/db.sqlite3", &db);

    if (con != SQLITE_OK) {
        fprintf(stderr, RED "Database Error: Cannot open database: %s\n" RESET, sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
        return -1;
    }

     con = sqlite3_exec(db, stmt, callback, NULL, &err_msg);

     if (con != SQLITE_OK) {
         fprintf(stderr, YELLOW "SQL error: %s\n" RESET, err_msg);
         sqlite3_free(err_msg);
         sqlite3_close(db);
         return -1;
     }

     sqlite3_close(db);

    return 0;
}
