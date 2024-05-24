#ifndef DB_H
#define DB_H

#include <mysql/mysql.h>

int dbConnect(char *server, char *user, const char *password, char *database);
int closeDB();
int dbInit();
int addData(double temp, double humidity);

#endif //DB_H
