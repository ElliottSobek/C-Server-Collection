/* References
 * http://zetcode.com/db/mysqlc/
 * https://dev.mysql.com/doc/refman/8.0/en/c-api.html
 */

// apt install libmysqlclient-dev?

#include <stdio.h>
#include <mysql.h>
#include <my_global.h>

// Create database
int main(int argc, char **argv) {
	MYSQL *con = mysql_init(NULL);

	if (con == NULL) {
		fprintf(stderr, "%s\n", mysql_error(con));
		return 1;
	}

	if (mysql_real_connect(con, "localhost", "root", "root_pswd", NULL, 0, NULL, 0) == NULL) {
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return 1;
	}

	if (mysql_query(con, "CREATE DATABASE testdb")) {
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return 1;
	}
	mysql_close(con);

	return 0;
}

// Create and add data to a table
void finish_with_error(MYSQL *con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

int main(int argc, char **argv) {
  MYSQL *con = mysql_init(NULL);

  if (con == NULL)  {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user12", "34klq*", "testdb", 0, NULL, 0) == NULL)
      finish_with_error(con);

  if (mysql_query(con, "DROP TABLE IF EXISTS Cars"))
      finish_with_error(con);

  if (mysql_query(con, "CREATE TABLE Cars(Id INT, Name TEXT, Price INT)"))
      finish_with_error(con);

  if (mysql_query(con, "INSERT INTO Cars VALUES(1,'Audi',52642)"))
      finish_with_error(con);

  if (mysql_query(con, "INSERT INTO Cars VALUES(2,'Mercedes',57127)"))
      finish_with_error(con);

  if (mysql_query(con, "INSERT INTO Cars VALUES(3,'Skoda',9000)"))
      finish_with_error(con);

  if (mysql_query(con, "INSERT INTO Cars VALUES(4,'Volvo',29000)"))
      finish_with_error(con);

  if (mysql_query(con, "INSERT INTO Cars VALUES(5,'Bentley',350000)"))
      finish_with_error(con);

  if (mysql_query(con, "INSERT INTO Cars VALUES(6,'Citroen',21000)"))
      finish_with_error(con);

  if (mysql_query(con, "INSERT INTO Cars VALUES(7,'Hummer',41400)"))
      finish_with_error(con);

  if (mysql_query(con, "INSERT INTO Cars VALUES(8,'Volkswagen',21600)"))
      finish_with_error(con);

  mysql_close(con);
  return 0;
}

// Retrieve data
// NOTE: int id = mysql_insert_id(con);
// printf("The last inserted row id is: %d\n", id);
void finish_with_error(MYSQL *con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

int main(int argc, char **argv) {
  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
      fprintf(stderr, "mysql_init() failed\n");
      exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user12", "34klq*", "testdb", 0, NULL, 0) == NULL)
      finish_with_error(con);

  if (mysql_query(con, "SELECT * FROM Cars"))
      finish_with_error(con);

  MYSQL_RES *result = mysql_store_result(con);

  if (result == NULL)
      finish_with_error(con);

  int num_fields = mysql_num_fields(result);
  MYSQL_ROW row;

  while ((row = mysql_fetch_row(result))) {
      for(int i = 0; i < num_fields; i++)
          printf("%s ", row[i] ? row[i] : "NULL");
      printf("\n");
  }

  mysql_free_result(result);
  mysql_close(con);
  return 0;
}

// Columb headers
void finish_with_error(MYSQL *con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

int main(int argc, char **argv){
  MYSQL *con = mysql_init(NULL);

  if (con == NULL)
      fprintf(stderr, "mysql_init() failed\n");
      return 1;

  if (mysql_real_connect(con, "localhost", "user12", "34klq*", "testdb", 0, NULL, 0) == NULL)
      finish_with_error(con);

  if (mysql_query(con, "SELECT * FROM Cars LIMIT 3"))
      finish_with_error(con);

  MYSQL_RES *result = mysql_store_result(con);

  if (result == NULL)
      finish_with_error(con);

  int num_fields = mysql_num_fields(result);
  MYSQL_ROW row;
  MYSQL_FIELD *field;

  while ((row = mysql_fetch_row(result))) {
      for(int i = 0; i < num_fields; i++) {
          if (i == 0)  {
             while(field = mysql_fetch_field(result))
                printf("%s ", field->name);
             printf("\n");
          }
          printf("%s  ", row[i] ? row[i] : "NULL");
      }
  }
  printf("\n");
  mysql_free_result(result);
  mysql_close(con);
  return 0;
}

// Multiple statements
void finish_with_error(MYSQL *con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

int main(int argc, char **argv) {
  int status = 0;
  MYSQL *con = mysql_init(NULL);

  if (con == NULL)
      fprintf(stderr, "mysql_init() failed\n");
      exit(1);

  if (mysql_real_connect(con, "localhost", "user12", "34klq*", "testdb", 0, NULL, CLIENT_MULTI_STATEMENTS) == NULL)
      finish_with_error(con);

  if (mysql_query(con, "SELECT Name FROM Cars WHERE Id=2;SELECT Name FROM Cars WHERE Id=3;SELECT Name FROM Cars WHERE Id=6"))
      finish_with_error(con);

  do {
      MYSQL_RES *result = mysql_store_result(con);

      if (result == NULL)
          finish_with_error(con);

      MYSQL_ROW row = mysql_fetch_row(result);

      printf("%s\n", row[0]);
      mysql_free_result(result);
      status = mysql_next_result(con);

      if (status > 0)
          finish_with_error(con);

  } while(status == 0);

  mysql_close(con);
  return 0;
}

// Inserting images
#include <my_global.h>
#include <mysql.h>
#include <string.h>

void finish_with_error(MYSQL *con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

int main(int argc, char **argv) {
  FILE *fp = fopen("woman.jpg", "rb");

  if (fp == NULL) {
      fprintf(stderr, "cannot open image file\n");
      return 1;
  }

  fseek(fp, 0, SEEK_END);

  if (ferror(fp)) {

      fprintf(stderr, "fseek() failed\n");
      int r = fclose(fp);

      if (r == EOF)
          fprintf(stderr, "cannot close file handler\n");
      return 1;
  }

  int flen = ftell(fp);

  if (flen == -1) {

      perror("error occurred");
      int r = fclose(fp);

      if (r == EOF)
          fprintf(stderr, "cannot close file handler\n");
      return 1;
  }

  fseek(fp, 0, SEEK_SET);

  if (ferror(fp)) {

      fprintf(stderr, "fseek() failed\n");
      int r = fclose(fp);

      if (r == EOF)
          fprintf(stderr, "cannot close file handler\n");
      return 1;
  }

  char data[flen+1];
  int size = fread(data, 1, flen, fp);

  if (ferror(fp)) {

      fprintf(stderr, "fread() failed\n");
      int r = fclose(fp);

      if (r == EOF)
          fprintf(stderr, "cannot close file handler\n");

      return 1;
  }
  int r = fclose(fp);

  if (r == EOF)
      fprintf(stderr, "cannot close file handler\n");

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
      fprintf(stderr, "mysql_init() failed\n");
  	  return 1;
  	}

  if (mysql_real_connect(con, "localhost", "user12", "34klq*", "testdb", 0, NULL, 0) == NULL)
      finish_with_error(con);

  char chunk[2*size+1];
  mysql_real_escape_string(con, chunk, data, size);

  char *st = "INSERT INTO Images(Id, Data) VALUES(1, '%s')";
  size_t st_len = strlen(st);

  char query[st_len + 2*size+1];
  int len = snprintf(query, st_len + 2*size+1, st, chunk);

  if (mysql_real_query(con, query, len))
      finish_with_error(con);

  mysql_close(con);
  return 0;
}

 // Selecting images
 void finish_with_error(MYSQL *con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

int main(int argc, char **argv) {
  FILE *fp = fopen("woman2.jpg", "wb");

  if (fp == NULL) {
      fprintf(stderr, "cannot open image file\n");
      return 1;
  }

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
      fprintf(stderr, "mysql_init() failed\n");
      return 1;
  }

  if (mysql_real_connect(con, "localhost", "user12", "34klq*", "testdb", 0, NULL, 0) == NULL)
      finish_with_error(con);

  if (mysql_query(con, "SELECT Data FROM Images WHERE Id=1"))
      finish_with_error(con);

  MYSQL_RES *result = mysql_store_result(con);

  if (result == NULL)
      finish_with_error(con);

  MYSQL_ROW row = mysql_fetch_row(result);
  unsigned long *lengths = mysql_fetch_lengths(result);

  if (lengths == NULL)
      finish_with_error(con);

  fwrite(row[0], lengths[0], 1, fp);

  if (ferror(fp)) {
      fprintf(stderr, "fwrite() failed\n");
      mysql_free_result(result);
      mysql_close(con);
      return 1;
  }

  int r = fclose(fp);

  if (r == EOF)
      fprintf(stderr, "cannot close file handler\n");

  mysql_free_result(result);
  mysql_close(con);
  return 0;
}
