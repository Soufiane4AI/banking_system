#ifndef BANQUE_SYSTEM_DBCONNECT_H
#define BANQUE_SYSTEM_DBCONNECT_H
#include <mysql.h>

MYSQL* connect_db();
void close_db(MYSQL *conn);

#endif 