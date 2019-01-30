/* References:
 * http://zetcode.com/db/sqlitec/
 * http://www.sqlite.org/docs.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "types.h"
#include "colors.h"

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

// // Makefile link -lsqlite3?

// // Gets sqlite version
// int main(void) {
// 	sqlite3 *db;
// 	sqlite3_stmt *res;

// 	int rc = sqlite3_open(":memory:", &db);

	// if (rc != SQLITE_OK) {
	// 	fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
	// 	sqlite3_close(db);
	// 	return 1;
	// }

// 	rc = sqlite3_prepare_v2(db, "SELECT SQLITE_VERSION()", -1, &res, 0);

	// if (rc != SQLITE_OK) {
	// 	fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
	// 	sqlite3_close(db);
	// 	return 1;
	// }

// 	rc = sqlite3_step(res);

	// if (rc == SQLITE_ROW)
	// 	printf("%s\n", sqlite3_column_text(res, 0));

	// sqlite3_finalize(res);
	// sqlite3_close(db);

	// return 0;
// }

// // Insert data
// // NOTE: sqlite3_last_insert_row()
// int main(void) {
// 	sqlite3 *db;
// 	char *err_msg = 0;

// 	int rc = sqlite3_open("test.db", &db);

// 	if (rc != SQLITE_OK) {
// 		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
// 		sqlite3_close(db);
// 		return 1;
// 	}

// 	char *sql = "DROP TABLE IF EXISTS Cars;"
// 				"CREATE TABLE Cars(Id INT, Name TEXT, Price INT);"
// 				"INSERT INTO Cars VALUES(1, 'Audi', 52642);"

// 	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

// 	if (rc != SQLITE_OK) {
// 		fprintf(stderr, "SQL error: %s\n", err_msg);
// 		sqlite3_free(err_msg);
// 		sqlite3_close(db);
// 		return 1;
// 	}

// 	sqlite3_close(db);

// 	return 0;
// }

// // Select data
// int main(void) {
// 	sqlite3 *db;
// 	char *err_msg = 0;

// 	int rc = sqlite3_open("test.db", &db);

// 	if (rc != SQLITE_OK) {
// 		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
// 		sqlite3_close(db);
// 		return 1;
// 	}

// 	char *sql = "SELECT * FROM Cars;";

// 	rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

// 	if (rc != SQLITE_OK) {
// 		fprintf(stderr, "Failed to select data\nSQL error: %s\n", err_msg);
// 		sqlite3_free(err_msg);
// 		sqlite3_close(db);
// 		return 1;
// 	}
// 	sqlite3_close(db);

// 	return 0;
// }

// int callback(void *NotUsed, int argc, char **argv, char **azColName) {
// 	NotUsed = 0;

// 	for (int i = 0; i < argc; i++)
// 		printf("%s = %s\n", azColName[i], argv[i] ? argv[i]: "NULL");
// 	printf("\n");

// 	return 0;
// }

// // Parameterized Query
// // NOTE: char &sql = "SELECT * FROM Cars WHERE Id = @id;";
// int main(void) {
// 	sqlite3 *db;
// 	char *err_msg = 0;
// 	sqlite3_stmt *res;

// 	int rc = sqlite3_open("test.db", &db);

// 	if (rc != SQLITE_OK) {
// 		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
// 		sqlite3_close(db);
// 		return 1;
// 	}

// 	char *sql = "SELECT * FRMO Cars WHERE Id = ?;";
// 	rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

// 	if (rc == SQLITE_OK)
// 		sqlite3_bind_int(res, 1, 3);
// 	else
// 		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));

// 	int step = sqlite3_step(res);

// 	if (step == SQLITE_ROW)
// 		printf("%s: %s\n", sqlite3_column_text(res, 0), sqlite3_column_text(res, 1));

// 	sqlite3_finalize(res);
// 	sqlite3_close(db);

// 	return 0;
// }

// // Insert Image
// int main(int argc, char **argv) {
//     FILE *fp = fopen("woman.jpg", "rb");

//     if (fp == NULL) {
//         fprintf(stderr, "Cannot open image file\n");
//         return 1;
//     }

//     fseek(fp, 0, SEEK_END);

//     if (ferror(fp)) {
//         fprintf(stderr, "fseek() failed\n");
//         int r = fclose(fp);

//         if (r == EOF)
//             fprintf(stderr, "Cannot close file handler\n");

//         return 1;
//     }

//     int flen = ftell(fp);

//     if (flen == -1) {
//         perror("error occurred");
//         int r = fclose(fp);

//         if (r == EOF)
//             fprintf(stderr, "Cannot close file handler\n");

//         return 1;
//     }

//     fseek(fp, 0, SEEK_SET);

//     if (ferror(fp)) {
//         fprintf(stderr, "fseek() failed\n");
//         int r = fclose(fp);

//         if (r == EOF)
//             fprintf(stderr, "Cannot close file handler\n");

//         return 1;
//     }

//     char data[flen+1];

//     int size = fread(data, 1, flen, fp);

//     if (ferror(fp)) {
//         fprintf(stderr, "fread() failed\n");
//         int r = fclose(fp);

//         if (r == EOF)
//             fprintf(stderr, "Cannot close file handler\n");

//         return 1;
//     }

//     int r = fclose(fp);

//     if (r == EOF)
//         fprintf(stderr, "Cannot close file handler\n");


//     sqlite3 *db;
//     char *err_msg = 0;
//     int rc = sqlite3_open("test.db", &db);

//     if (rc != SQLITE_OK) {
//         fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
//         sqlite3_close(db);
//         return 1;
//     }
//     sqlite3_stmt *pStmt;

//     char *sql = "INSERT INTO Images(Data) VALUES(?)";
//     rc = sqlite3_prepare(db, sql, -1, &pStmt, 0);

//     if (rc != SQLITE_OK) {
//         fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
//         return 1;
//     }
//     sqlite3_bind_blob(pStmt, 1, data, size, SQLITE_STATIC);

//     rc = sqlite3_step(pStmt);

//     if (rc != SQLITE_DONE)
//         printf("execution failed: %s", sqlite3_errmsg(db));

//     sqlite3_finalize(pStmt);
//     sqlite3_close(db);

//     return 0;
// }

// // Reading Image
// int main(void) {
//     FILE *fp = fopen("woman2.jpg", "wb");

//     if (fp == NULL) {
//         fprintf(stderr, "Cannot open image file\n");
//         return 1;
//     }

//     sqlite3 *db;
//     char *err_msg = 0;
//     int rc = sqlite3_open("test.db", &db);

//     if (rc != SQLITE_OK) {
//         fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
//         sqlite3_close(db);
//         return 1;
//     }

//     char *sql = "SELECT Data FROM Images WHERE Id = 1";

//     sqlite3_stmt *pStmt;
//     rc = sqlite3_prepare_v2(db, sql, -1, &pStmt, 0);

//     if (rc != SQLITE_OK ) {
//         fprintf(stderr, "Failed to prepare statement\n");
//         fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
//         sqlite3_close(db);
//         return 1;
//     }

//     rc = sqlite3_step(pStmt);
//     int bytes = 0;

//     if (rc == SQLITE_ROW)
//         bytes = sqlite3_column_bytes(pStmt, 0);

//     fwrite(sqlite3_column_blob(pStmt, 0), bytes, 1, fp);

//     if (ferror(fp)) {
//         fprintf(stderr, "fwrite() failed\n");
//         return 1;
//     }
//     int r = fclose(fp);

//     if (r == EOF)
//         fprintf(stderr, "Cannot close file handler\n");

//     rc = sqlite3_finalize(pStmt);

//     sqlite3_close(db);

//     return 0;
// }

// // Metadata
// int main(void) {
//     sqlite3 *db;
//     char *err_msg = 0;
//     int rc = sqlite3_open("test.db", &db);

//     if (rc != SQLITE_OK) {
//         fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
//         sqlite3_close(db);
//         return 1;
//     }

//     char *sql = "PRAGMA table_info(Cars)";
//     rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

//     if (rc != SQLITE_OK ) {
//         fprintf(stderr, "Failed to select data\nSQL error: %s\n", err_msg);
//         sqlite3_free(err_msg);
//         sqlite3_close(db);
//         return 1;
//     }

//     sqlite3_close(db);

//     return 0;
// }

// int callback(void *NotUsed, int argc, char **argv, char **azColName) {
//     NotUsed = 0;

//     for (int i = 0; i < argc; i++)
//         printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//     printf("\n");

//     return 0;
// }

// // Transaction
// int main(void) {
//     sqlite3 *db;
//     char *err_msg = 0;
//     int rc = sqlite3_open("test.db", &db);

//     if (rc != SQLITE_OK) {
//         fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
//         sqlite3_close(db);
//         return 1;
//     }

//     char *sql = "DROP TABLE IF EXISTS Friends;"
//                 "BEGIN TRANSACTION;"
//                 "CREATE TABLE Friends(Id INTEGER PRIMARY KEY, Name TEXT);"
//                 "INSERT INTO Friends(Name) VALUES ('Tom');"
//                 "INSERT INTO Friends(Name) VALUES ('Rebecca');"
//                 "INSERT INTO Friends(Name) VALUES ('Jim');"
//                 "INSERT INTO Friend(Name) VALUES ('Robert');"
//                 "COMMIT;";

//     rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

//     if (rc != SQLITE_OK ) {
//         fprintf(stderr, "SQL error: %s\n", err_msg);
//         sqlite3_free(err_msg);
//         sqlite3_close(db);
//         return 1;
//     }
//     sqlite3_close(db);

//     return 0;
// }
