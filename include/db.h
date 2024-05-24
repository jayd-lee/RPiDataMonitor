#ifndef DB_H
#define DB_H

#include <mysql/mysql.h>

int dbConnect(char *server, char *user, const char *password, char *database);
int closeDB();
int dbInit();
int addData(double temp, double humidity);
int getAllStats(const char *startTime, const char *endTime,
		double *minHumidity, double *maxHumidity, double *averageHumidity,
		double *minTemp, double *maxTemp, double *averageTemp);

#endif //DB_H
