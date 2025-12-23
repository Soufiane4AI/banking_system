#ifndef  BANQUE_SYSTEM_INTERFACE_H
#define BANQUE_SYSTEM_INTERFACE_H

typedef struct {
    char cin[20],nom[50],prenom[50],
    n_tele[15],adresse[100],num_account[17];
    double solde;
    int is_logged_in;
} ActiveSession;

int login_user(MYSQL *conn, char *compte, char *password, ActiveSession *session);
void menu_utilisateur(MYSQL *conn, ActiveSession *session);
void connecter_compte(MYSQL *conn, ActiveSession *session);
void menu_admin(MYSQL *conn , ActiveSession *session);
void menu_general(MYSQL *conn, ActiveSession *session);
#endif 