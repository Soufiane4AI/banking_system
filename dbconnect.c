#include <stdio.h>
#include <mysql.h>
#include "dbconnect.h"

/*----------------------------------------------------------*/

MYSQL* connect_db() {
    MYSQL *conn = mysql_init(NULL);
    MYSQL *conn_database = mysql_real_connect(conn,"localhost","root","SfN@Hamm2005","banque_system",3306,NULL,0);
    if (conn == NULL) {
        printf("Un probleme dans le systeme !\n"); // message d'erreur en background
        return NULL;
    }
    if (conn_database == NULL){
        printf("Un probleme dans le systeme !\n");
        mysql_close(conn);
        return NULL;
    }
    return conn;
}

/*----------------------------------------------------------*/

void close_db(MYSQL *conn) {
    if (conn != NULL) {
        mysql_close(conn);
    }
}