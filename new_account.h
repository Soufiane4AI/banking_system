#ifndef BANQUE_SYSTEM_ACREATION_H
#define BANQUE_SYSTEM_ACREATION_H

#include <mysql.h>
#include "interface.h"

typedef struct Client
{
    char CIN[20],nom[50],prenom[50],
    n_tele[15],date_naissance[20],adresse[100],
    mdp[9],n_compte[17];
}Client;

char* generer_n_compte(MYSQL *conn);
char* generer_mdp(void);
void client_info(MYSQL *conn, Client *C);
Client* creer_client(void);
void inserer_client_db(MYSQL *conn,Client *C);
void creer_compte(MYSQL *conn, ActiveSession *session);

#endif