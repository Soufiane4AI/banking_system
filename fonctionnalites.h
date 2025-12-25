#ifndef BANQUE_SYSTEM_FONCTIONNALITES_H
#define BANQUE_SYSTEM_FONCTIONNALITES_H

#include <mysql.h>
#include "interface.h"


int effectuer_transaction(MYSQL *conn, ActiveSession *session, char *compte_dest, double montant);
int effectuer_retrait_depot(MYSQL *conn, ActiveSession *session, double montant,int retrait_depot);
int effectuer_depot(MYSQL *conn, ActiveSession *session, double montant);
int effectuer_retrait(MYSQL *conn, ActiveSession *session, double montant);
void afficher_releve(MYSQL *con, char *mon_compte);
void modifier_informations(MYSQL *conn, ActiveSession *session);
int recuperer_mdp(MYSQL *conn, char *mdp_recupere);

#endif
