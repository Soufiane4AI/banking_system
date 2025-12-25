#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mysql.h>
#include "fonctionnalites.h"
#include "interface.h"
#include "dbconnect.h"


//  Fonction pour effectuer un virement entre deux comptes 

int effectuer_transaction(MYSQL *conn, ActiveSession *session, char *compte_dest, double montant) {
    char query[1024];
    double solde_source;
    MYSQL_RES *result;
    MYSQL_ROW row;
    time_t now = time(NULL);
    char date_str[20];
    struct tm *timeinfo = localtime(&now);
    
    // Formater la date actuelle (YYYY-MM-DD HH:MM:SS)
    strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    // Validation du montant
    if (montant <= 0) {
        printf(" Le montant doit etre strictement positif.\n");
        return 0;
    } 
   
    // Début de la transaction SQL 
    if (mysql_query(conn, "START TRANSACTION")) {
        fprintf(stderr, " Un probleme dans le systeme impossible de demarrer la transaction.\n");
        return 0;
    }
    // Vérifier que le compte destinataire existe
    sprintf(query, "SELECT num_account FROM accounts WHERE num_account = '%s'", compte_dest);
    if (mysql_query(conn, query)) {
        mysql_query(conn, "ROLLBACK");
        fprintf(stderr, "Un probleme dans le systeme lors de la verification du compte destinataire.\n");
        return 0;
    }
    result = mysql_store_result(conn);
    if (result == NULL || mysql_num_rows(result) == 0) {
        if (result != NULL)
        mysql_free_result(result);
       
        mysql_query(conn, "ROLLBACK");
        printf(" Le compte destinataire existe pas.\n");
        return 0;
    }
    mysql_free_result(result);
    // Vérifier que le solde est suffisant
    sprintf(query, "SELECT solde FROM accounts WHERE num_account = '%s'", session->num_account);
    if (mysql_query(conn, query)) {
        mysql_query(conn, "ROLLBACK");
        fprintf(stderr, "Un probleme dans le systeme lors de la verification du solde.\n");
        return 0;
    }
    
    result = mysql_store_result(conn);
    row = mysql_fetch_row(result);
    solde_source = atof(row[0]);
    mysql_free_result(result);
    
    if (solde_source < montant) {
        mysql_query(conn, "ROLLBACK");
        printf(" Solde insuffisant. Solde actuel: %.2f DH\n", solde_source);
        return 0;
    }
    // Débiter le compte source
    sprintf(query, "UPDATE accounts SET solde = solde - %.2f WHERE num_account = '%s'", 
            montant, session->num_account);
    if (mysql_query(conn, query)) {
        mysql_query(conn, "ROLLBACK");
        fprintf(stderr, " Un probleme dans le systeme impossible de debiter le compte.\n");
        return 0;
    }
    // Créditer le compte destinataire
    sprintf(query, "UPDATE accounts SET solde = solde + %.2f WHERE num_account = '%s'", 
            montant, compte_dest);
    if (mysql_query(conn, query)) {
        mysql_query(conn, "ROLLBACK");
        fprintf(stderr, " Un probleme dans le systeme impossible de crediter le compte destinataire.\n");
        return 0;
    }
    // Enregistrer l'opération dans l'historique
    sprintf(query, "INSERT INTO operations (date, type_op, amount, account_from, account_to) "
                   "VALUES ('%s', 'TRANSACTION', %.2f, '%s', '%s')",
            date_str, montant, session->num_account, compte_dest);
    if (mysql_query(conn, query)) {
        mysql_query(conn, "ROLLBACK");
        fprintf(stderr, "Un probleme dans le systeme impossible d'enregistrer l'operation.\n");
        return 0;
    }
    // Valider la transaction 
    if (mysql_query(conn, "COMMIT")) {
        mysql_query(conn, "ROLLBACK");
        fprintf(stderr, " Impossible de valider la transaction.\n");
        return 0;
    }

    session->solde -= montant;

    printf("\n========================================\n");
    printf("    TRANSACTION REUSSIE\n");
    printf("========================================\n");
    printf("Montant transfere : %.2f DH\n", montant);
    printf("Compte destinataire : %s\n", compte_dest);
    printf("Nouveau solde : %.2f DH\n", session->solde);
    printf("========================================\n\n");
    
    return 1;
}

// fonction gère les deux operations un retrait ou un dépôt

int effectuer_retrait_depot(MYSQL *conn, ActiveSession *session, double montant,int retrait_depot) {
    char query[1024];
    double solde_actuel;
    MYSQL_RES *result;
    MYSQL_ROW row;
    time_t now = time(NULL);
    char date[20];
    struct tm *timeinfo = localtime(&now);

    strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", timeinfo);

    if (montant < 50) {
        printf("Vous n'avez pas eteignez le fond minimal de %s !\n",retrait_depot ? "depot" : "retrait");
        return 0;
    }
    if (mysql_query(conn, "START TRANSACTION")) {return 0;}

    sprintf(query, "SELECT solde FROM accounts WHERE num_account = '%s'", session->num_account);
    if (mysql_query(conn, query)) {
        mysql_query(conn, "ROLLBACK");
        return 0;
    }

    result = mysql_store_result(conn);
    row = mysql_fetch_row(result);
    solde_actuel = atof(row[0]);
    mysql_free_result(result);

    if (retrait_depot==0 && solde_actuel < montant) {
        mysql_query(conn, "ROLLBACK");
        printf("Solde insuffisant. Solde actuel: %.2f DH\n", solde_actuel);
        return 0;
    }
    sprintf(query, "UPDATE accounts SET solde = solde %c %.2f WHERE num_account = '%s'",
            retrait_depot ? '+' : '-',montant, session->num_account);
    if (mysql_query(conn, query)) {
        mysql_query(conn, "ROLLBACK");
        printf( "Impossible d'effectuer le %s.\n",retrait_depot ?"DEPOT" : "RETRAIT");
        return 0;
    }

    sprintf(query, "INSERT INTO operations (date, type_op, amount, account_from, account_to) "
                         "VALUES ('%s', '%s', %.2f, '%s', NULL)",
                         date, retrait_depot ?"DEPOT" : "RETRAIT",montant, session->num_account);
    if (mysql_query(conn, query)) {
        mysql_query(conn, "ROLLBACK");
        return 0;
    }

    if (mysql_query(conn, "COMMIT")) {
        mysql_query(conn, "ROLLBACK");
        printf("Impossible de valider la transaction.\n");
        return 0;
    }
    if (retrait_depot) {
        session->solde += montant;
    } else {
        session->solde -= montant;
    }
    printf("\n========================================\n");
    printf("           %s REUSSI\n",retrait_depot ?"DEPOT" : "RETRAIT");
    printf("========================================\n");
    printf("Montant %s : %.2f DH\n",retrait_depot ?"depose" : "retire" ,montant);
    printf("Nouveau solde : %.2f DH\n", session->solde);
    printf("========================================\n\n");

    return 1;
}

// Ces deux fonctions precisent si c'est un depot=1 ou un retrait=0

int effectuer_depot(MYSQL *conn, ActiveSession *session, double montant) {
    return effectuer_retrait_depot(conn, session, montant, 1);
}

/*----------------------------------------------------------*/
int effectuer_retrait(MYSQL *conn, ActiveSession *session, double montant) {
    return effectuer_retrait_depot(conn, session, montant, 0);
}


// fonction pour afficher le relevé bancaire

void afficher_releve(MYSQL *con, char *mon_compte) {
    char query[1024];
    MYSQL_RES *result;
    MYSQL_ROW row;

    sprintf(query,
            "SELECT date, type_op, amount, COALESCE(account_to, 'Guichet Automatique') "
            "FROM operations "
            "WHERE account_from = '%s' "
            "ORDER BY date DESC",
            mon_compte);

    if (mysql_query(con, query)) 
    return;
    result = mysql_store_result(con);

    if (result == NULL) {
        fprintf(stderr, "Un probleme dans le systeme !\n");
        return;
    }
 
    printf("\n");
    printf("==================================================================================\n");
    printf("   RELEVE BANCAIRE : %s\n", mon_compte);
    printf("==================================================================================\n");
    printf("| %-22s | %-15s | %-12s | %-20s |\n", "DATE", "OPERATION", "MONTANT", "DESTINATAIRE");
    printf("|------------------------|-----------------|--------------|----------------------|\n");

    while ((row = mysql_fetch_row(result))) {
        printf("| %-22s | %-15s | %-9s DH | %-20s |\n",
               row[0], row[1], row[2], row[3]);
    }

    printf("==================================================================================\n\n");

    mysql_free_result(result);
}


// fonction pour Modifier les informations personnelles

void modifier_informations(MYSQL *conn, ActiveSession *session) {
    int choix = 0;
    char query[512];
    char nouvelle_valeur[150];
    
    printf("\n========================================\n");
    printf("   MODIFICATION D'INFORMATIONS\n");
    printf("========================================\n");
    printf("1. Modifier mot de passe\n");
    printf("2. Modifier numero telephone\n");
    printf("3. Modifier adresse\n");
    printf("4. Retour au menu\n");
    printf("----------------------------------------\n");
    printf("Votre choix : ");
    
    if (scanf("%d", &choix) != 1) {
        while(getchar() != '\n');
        printf(" Entree invalide.\n");
        return;
    }
    while(getchar() != '\n');
    
    switch(choix) {
        case 1: {  
            printf("\nNouveau mot de passe : ");
            fgets(nouvelle_valeur, sizeof(nouvelle_valeur), stdin);
            nouvelle_valeur[strcspn(nouvelle_valeur, "\n")] = 0;
            
            sprintf(query, "UPDATE accounts SET mdp = '%s' WHERE num_account = '%s'",
                    nouvelle_valeur, session->num_account);
            
            if (mysql_query(conn, query)) {
                printf("Un probleme dans le systeme , recommencer plutard\n");
            } else {
                printf(" Mot de passe modifie avec succes.\n");
            }
            break;
        }
        
        case 2: { 
            printf("\nNouveau numero de telephone : ");
            fgets(nouvelle_valeur, sizeof(nouvelle_valeur), stdin);
            nouvelle_valeur[strcspn(nouvelle_valeur, "\n")] = 0;
            
            sprintf(query, "UPDATE client SET n_tele = '%s' WHERE CIN = '%s'",
                    nouvelle_valeur, session->cin);
            
            if (mysql_query(conn, query)) {
                printf("Un probleme dans le systeme , recommencer plutard\n");
            } else {
                strcpy(session->n_tele, nouvelle_valeur);
                printf(" Numero de telephone modifie avec succes.\n");
            }
            break;
        }
        
        case 3: {  
            printf("\nNouvelle adresse : ");
            fgets(nouvelle_valeur, sizeof(nouvelle_valeur), stdin);
            nouvelle_valeur[strcspn(nouvelle_valeur, "\n")] = 0;
            
            sprintf(query, "UPDATE client SET adresse = '%s' WHERE CIN = '%s'",
                    nouvelle_valeur, session->cin);
            
            if (mysql_query(conn, query)) {
                printf("Un probleme dans le systeme , recommencer plutard\n");
            } else {
                strcpy(session->adresse, nouvelle_valeur);
                printf(" Adresse modifiee avec succes.\n");
            }
            break;
        }
        
        case 4: { 
            printf("Retour au menu principal.\n");
            break;
        }
        
        default: {
            printf("Choix invalide.\n");
            break;
        }
    }
}


// fonction pour Recuperation de mot de passe oublié    

int recuperer_mdp(MYSQL *conn, char *mdp_recupere) {
    char query[1024];
    MYSQL_RES *result;
    MYSQL_ROW row;
    char input_nom[50];
    char input_prenom[50];
    char input_cin[20];
    char input_tele[20];
    
    printf("\n--- RECUPERATION MOT DE PASSE ---\n");
    printf("Veuillez entrer vos informations personnelles :\n");
    printf("Nom : ");
    scanf(" %[^\n]", input_nom);
    while(getchar() != '\n');
    
    printf("Prenom : ");
    scanf(" %[^\n]", input_prenom);
    while(getchar() != '\n');
    
    printf("CIN : ");
    scanf(" %[^\n]", input_cin);
    while(getchar() != '\n');
    
    printf("Numero de telephone : ");
    scanf(" %[^\n]", input_tele);
    while(getchar() != '\n');
    
    // Vérifier dans la base de données
    sprintf(query,
            "SELECT ac.num_account, ac.mdp "
            "FROM accounts AS ac "
            "INNER JOIN client AS cl ON ac.titulaire = cl.CIN "
            "WHERE cl.nom = '%s' AND cl.prenom = '%s' AND cl.CIN = '%s' AND cl.n_tele = '%s'",
            input_nom, input_prenom, input_cin, input_tele);
    
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Un probleme dans le systeme ! \n");
        return 0;
    }
    
    result = mysql_store_result(conn);
    
    if (result == NULL || mysql_num_rows(result) == 0) {
        printf("\n Informations introuvables dans la base de donnees.\n");
        mysql_free_result(result);
        return 0;
    }
    
    row = mysql_fetch_row(result);
    char *num_account = row[0];
    strcpy(mdp_recupere, row[1]);
    
    printf("\n========================================\n");
    printf("   MOT DE PASSE RECUPERE !\n");
    printf("========================================\n");
    printf("Numero de compte: %s\n", num_account);
    printf("Mot de passe: %s\n", mdp_recupere);
    printf("========================================\n\n");
    
    mysql_free_result(result);
    return 1;
}
