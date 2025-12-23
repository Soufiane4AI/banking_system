#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mysql.h>
#include "dbconnect.h"
#include "interface.h"

int main(void) {
    srand(time(NULL));
    MYSQL *conn = connect_db();  
    ActiveSession* session = (ActiveSession*)malloc(sizeof(ActiveSession));
    if (conn == NULL) {
        printf(" Un probleme dans le systeme !\n"); 
        return 1;
    }
    if (session == NULL) {
        printf(" Un probleme dans le systeme !\n"); 
        close_db(conn);
        return 1;
    }
    // Appel de la fonction qui gère tout le système bancaire
    menu_general(conn , session);
    
    free(session);
    close_db(conn);
    
    return 0;
}

