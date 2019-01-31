#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "types.h"
#include "resultset.h"

#define STR_MAX 2048

ResultSet create_rs(const int rows, const String *column_names, const S_Ll row_data, const unsigned long result_amount) {
    ResultSet rs = (ResultSet) malloc(sizeof(resultset_t));

    if (!rs)
        return NULL;
    rs->column_names = (String*) malloc(rows * sizeof(String));

    if (!rs->column_names) {
        free(rs);
        rs = NULL;

        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        rs->column_names[i] = (String) calloc(strnlen(column_names[i], STR_MAX) + NT_LEN, sizeof(char));

        if (!rs->column_names[i]) {
            free(rs);
            rs = NULL;

            free(rs->column_names);
            rs->column_names = NULL;

            for (int j = 0; j < i; j++) {
                free(rs->column_names[j]);
                rs->column_names[j] = NULL;
            }
            return NULL;
        }
    }
    rs->query_result = row_data;
    rs->column_count = rows;
    rs->result_amount = result_amount;

    return rs;
}

ResultSet parse_query_result(const int rows, sqlite3_stmt *sql_byte_code) {
    String column_names[rows];

    sqlite3_step(sql_byte_code);

    for (int i = 0; i < rows; i++)
        column_names[i] = (String) sqlite3_column_name(sql_byte_code, i);

    unsigned long result_amount = 0;
    String row_data[rows + NT_LEN], row_datum;
    S_Ll result_rows = s_ll_create();

    if (!result_rows)
        return NULL;

    while (sqlite3_step(sql_byte_code) == SQLITE_ROW) {
        for (int i = 0; i < rows; i++) {
            row_data[i] = "";
            row_datum = (String) sqlite3_column_text(sql_byte_code, i);

            if (!row_datum)
                row_data[i] = "NULL";
            else
                row_data[i] = row_datum;
        }
        row_data[rows + NT_LEN] = NULL;

        s_ll_insert(result_rows, (Generic) row_data, STRING_ARRAY); // Check return value?
        result_amount++;
    }
    return create_rs(rows, column_names, result_rows, result_amount);
}

void sqlite_destroy_rs(ResultSet rs) {
    if (rs->query_result)
        s_ll_destroy(rs->query_result);

    for (unsigned short i = 0; i < rs->column_count; i++) {
        free(rs->column_names[i]);
        rs->column_names[i] = NULL;
    }
    free(rs->column_names);
    rs->column_names = NULL;

    free(rs);
    rs = NULL;

    return;
}

void sqlite_print_rs(ResultSet rs) {
    const unsigned short column_count = rs->column_count;

    for (unsigned short i = 0; i < column_count; i++)
        printf(" %s ", rs->column_names[i]);
    printf("\n");
    const S_Ll query_result = rs->query_result;

    for (S_Ll_Node node = query_result->root; node; node = node->next) {
        for (unsigned short i = 0; i < column_count; i++)
            printf(" %s ", ((String*) node->value)[i]);
        printf("\n");
    }
}
