#include "db.h"
#include <stdio.h>
#include <stdlib.h>

MYSQL *conn;
MYSQL_RES *res;

int dbConnect(char *server, char *user, const char *password, char *database)
{
	conn = mysql_init(NULL);
	/* Connect to database */
	if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}

	return 0;
}

int closeDB() {
	/* Close connection */
	mysql_free_result(res);
	mysql_close(conn);
	return 0;
}

int dbInit() {
	if (mysql_query(conn, "create database if not exists RPiDB;"))
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
	if (mysql_query(conn, "use RPiDB;"))
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
	if (mysql_query(conn, "create table if not exists rpiData (time TIMESTAMP, temperature_C DOUBLE, humidity_percent DOUBLE);"))
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
	return 0;
}

int addData(double humidity, double temp) {
	char buffer[100];
	sprintf(buffer, "insert into rpiData values (NOW(), %lf, %lf);", temp, humidity);

	if (mysql_query(conn, buffer))
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
	return 0;
}


   // /* send SQL query */
   // if (mysql_query(conn, "show tables"))
   // {
   //     fprintf(stderr, "%s\n", mysql_error(conn));
   //     exit(1);
   // }
   // res = mysql_use_result(conn);

   // /* output table name */
   // printf("MySQL Tables in studentdb database:\n");
   // while ((row = mysql_fetch_row(res)) != NULL)
   //     printf("%s \n", row[0]);

   // /* send SQL query */
   // if (mysql_query(conn, "select name from class101"))
   // {
   //     fprintf(stderr, "%s\n", mysql_error(conn));
   //     exit(1);
   // }
   // res = mysql_use_result(conn);

   // /* output table name */
   // printf("Data in class101:\n");
   // while ((row = mysql_fetch_row(res)) != NULL)
   //     printf("%s \n", row[0]);
   // if (mysql_query(conn, "update class101 set gpa = 4.98 where id = 11"))
   // {
   //     fprintf(stderr, "%s\n", mysql_error(conn));
   //     exit(1);
   // }
