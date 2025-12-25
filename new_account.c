#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "new_account.h"
#include "interface.h"


// fonction pour générer un numéro de compte unique

char* generer_n_compte(MYSQL *conn) {
    char *num = (char*)malloc(17 * sizeof(char));
    char query[256];
    MYSQL_RES *result;
    int existe = 1;
    
    if (num == NULL) {
        return NULL;
    }
    
    while (existe) {
        for (int i = 0; i < 16; i++) {
            num[i] = '0' + (rand() % 10);
        }
        num[16] = '\0';
        
        // Vérifier si ce numéro existe déjà dans la BD
        sprintf(query, "SELECT num_account FROM accounts WHERE num_account = '%s'", num);
        
        if (mysql_query(conn, query)) {
            free(num);
            return NULL;
        }
        
        result = mysql_store_result(conn);
        if (result == NULL) {
            free(num);
            return NULL;
        }
        
        if (mysql_num_rows(result) == 0) {
            existe = 0;  
        }
        
        mysql_free_result(result);
    }
    
    return num;
}

// fonction pour générer un mot de passe aléatoire

char* generer_mdp(void) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *mdp = (char*)malloc(9 * sizeof(char));
    for (int i = 0; i < 8; i++) {
        mdp[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    mdp[8] = '\0';
    return mdp;
}

// fonction pour saisir les données du client retourne 1 si tous est passé correctement

int client_info(MYSQL *conn, Client *C) {
    char *n_compte;
    char *mdp;
    char query[256];
    MYSQL_RES *result;

    printf("Veuillez entrer le Prenom : ");
    scanf(" %[^\n]", C->prenom);
    getchar() ;
    printf("Veuillez entrer le Nom : ");
    scanf(" %[^\n]", C->nom);
    getchar() ;   
    printf("Veuillez entrer la CIN : ");
    scanf(" %[^\n]", C->CIN);
    getchar();
    printf("Veuillez entrer la Date de naissance : ");
    scanf(" %[^\n]", C->date_naissance);
    getchar();
    printf("Veuillez entrer le Numero du telephone : ");
    scanf(" %[^\n]", C->n_tele);
    getchar();
    printf("Veuillez entrer un Adresse : ");
    scanf(" %[^\n]", C->adresse);
    getchar(); 

    sprintf(query, "SELECT CIN FROM client WHERE CIN = '%s'", C->CIN);
    
    if (mysql_query(conn, query)) {
        printf("Un probleme dans le systeme !\n");
        return 0;
    }   
    result = mysql_store_result(conn);

    if (mysql_num_rows(result) == 0){
        n_compte = generer_n_compte(conn);
        mdp = generer_mdp();
        mysql_free_result(result);
        if (n_compte == NULL || mdp == NULL) {
            printf("Un probleme dans le systeme !\n");
            free(n_compte);
            free(mdp);
            return 0;
        }else {
            strcpy(C->n_compte, n_compte);
            strcpy(C->mdp, mdp);
            printf("\n========================================\n");
            printf("   COMPTE CREE AVEC SUCCES !\n");
            printf("========================================\n");
            printf("Numero de compte: %s\n", n_compte);
            printf("Mot de passe: %s\n", mdp);
            printf("========================================\n\n");
            free(n_compte);
            free(mdp);
            return 1; 
        }
    }else {
        printf("\n CIN existe deja . Impossible de creer un compte avec ce CIN .\n");
        mysql_free_result(result);
        return 0;
    }

}

// fonction pour créer un nouveau client

Client* creer_client(void) {
    Client* C = (Client*)malloc(sizeof(Client));
    if (C == NULL) {
        printf("Erreur d'allocation memoire\n");
        return NULL;
    }
    return C;
}

// fonction pour insérer un nouveau client

void inserer_client_db(MYSQL *conn,Client *C) {
    char query[500];
    sprintf(query, "INSERT INTO client (nom,prenom,CIN,n_tele,date_naissance,adresse) VALUES ('%s', '%s','%s', '%s','%s', '%s')",
                    C->nom,C->prenom, C->CIN, C->n_tele,C->date_naissance, C->adresse);
    if (mysql_query(conn, query)) {return;}
    else{printf("Vous Etes ajoute avec succes!\n");}
    sprintf(query, "INSERT INTO accounts (num_account,titulaire,solde,mdp) "
                   "VALUES ('%s', '%s',%d, '%s')",C->n_compte, C->CIN, 0,C->mdp);
    if (mysql_query(conn, query)){return;}
}

// fonction pour Creer un compte   

void creer_compte(MYSQL *conn, ActiveSession *session) {
    Client* C = creer_client();
    int choix = 0;
    char input_compte[17]; // au cas où l'utilisateur veut se connecter à son compte directement après la création
    char input_mdp[9]; // de méme 

    printf("\n--- CREATION DE COMPTE ---\n\n");

    if (C != NULL) {
        if (!client_info(conn, C)) {         // Si la création a échoué (CIN existe ou erreur)
            free(C);
            return; 
        }
        
        inserer_client_db(conn, C);

        printf("Voulez-vous continuez ?\n");
        printf("Tapez 1 pour entrer au compte\n");
        printf("Tapez autre caractere pour quitter...\n");
        printf("-----------------------------------\n");
        printf("Votre choix : ");
        scanf("%d", &choix);
        while(getchar() != '\n');

        if (choix != 1) {
            printf("\n===================================\n");
            printf("Merci d'avoir choisi notre banque !\n");
            printf("Au revoir !\n");
            printf("===================================\n\n");
            free(C);
            exit(0);
        }else {
            strcpy(input_compte, C->n_compte);
            strcpy(input_mdp, C->mdp);

            if (login_user(conn, input_compte, input_mdp, session)) {
                free(C);
                menu_utilisateur(conn, session);
            } else {
                printf("\nImpossible de se connecter au nouveau compte,Reessayer plutard! \n");
                free(C);
            }
        }
    }else {
        printf("Un probleme dans le systeme ! \n");
    }
}
