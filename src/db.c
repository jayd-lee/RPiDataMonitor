#include "db.h"
#include <stdio.h>
#include <stdlib.h>

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;

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
	char query[100];
	sprintf(query, "insert into rpiData values (NOW(), %lf, %lf);", temp, humidity);

	if (mysql_query(conn, query))
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
	return 0;
}

int getMin(const char *column, const char *startTime, const char *endTime, double *minValue) {
	char query[200];
	sprintf(query, "SELECT MIN(%s) FROM rpiData WHERE DATE(time) >= '%s' AND DATE(time) <= '%s';", column, startTime, endTime);

	if (mysql_query(conn, query) != 0) {
		fprintf(stderr, "SELECT MIN(%s) failed. Error: %s\n", column, mysql_error(conn));
		return 1;
	}
	MYSQL_RES *res = mysql_store_result(conn);
	if (res == NULL) {
		fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
		return 1;
	}
	MYSQL_ROW row = mysql_fetch_row(res);
	*minValue = row ? atof(row[0]) : 0.0;
	mysql_free_result(res);
	return 0;
}

int getMax(const char *column, const char *startTime, const char *endTime, double *maxValue) {
	char query[200];
	sprintf(query, "SELECT MAX(%s) FROM rpiData WHERE DATE(time) >= '%s' AND DATE(time) <= '%s';", column, startTime, endTime);

	if (mysql_query(conn, query) != 0) {
		fprintf(stderr, "SELECT MAX(%s) failed. Error: %s\n", column, mysql_error(conn));
		return 1;
	}
	MYSQL_RES *res = mysql_store_result(conn);
	if (res == NULL) {
		fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
		return 1;
	}
	MYSQL_ROW row = mysql_fetch_row(res);
	*maxValue = row ? atof(row[0]) : 0.0;
	mysql_free_result(res);
	return 0;
}

int getAverage(const char *column, const char *startTime, const char *endTime, double *avgValue) {
	char query[200];
	sprintf(query, "SELECT AVG(%s) FROM rpiData WHERE DATE(time) >= '%s' AND DATE(time) <= '%s';", column, startTime, endTime);

	if (mysql_query(conn, query) != 0) {
		fprintf(stderr, "SELECT AVG(%s) failed. Error: %s\n", column, mysql_error(conn));
		return 1;
	}
	MYSQL_RES *res = mysql_store_result(conn);
	if (res == NULL) {
		fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
		return 1;
	}
	MYSQL_ROW row = mysql_fetch_row(res);
	*avgValue = row ? atof(row[0]) : 0.0;
	mysql_free_result(res);
	return 0;
}

int getAllStats(const char *startTime, const char *endTime,
		double *minHumidity, double *maxHumidity, double *averageHumidity,
		double *minTemp, double *maxTemp, double *averageTemp) {
	char defaultStartTime[] = "1970-01-01 00:00:00";  // Default start time
	char defaultEndTime[] = "9999-12-31 23:59:59";    // Default end time

	// If start time and end time are not provided, use default values
	startTime = startTime ? startTime : defaultStartTime;
	endTime = endTime ? endTime : defaultEndTime;

	if (getMin("humidity_percent", startTime, endTime, minHumidity) != 0) return 1;
	if (getMax("humidity_percent", startTime, endTime, maxHumidity) != 0) return 1;
	if (getAverage("humidity_percent", startTime, endTime, averageHumidity) != 0) return 1;

	if (getMin("temperature_C", startTime, endTime, minTemp) != 0) return 1;
	if (getMax("temperature_C", startTime, endTime, maxTemp) != 0) return 1;
	if (getAverage("temperature_C", startTime, endTime, averageTemp) != 0) return 1;

	return 0;
}

