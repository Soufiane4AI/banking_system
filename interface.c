#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mysql.h>
#include "dbconnect.h"
#include "acreation.h"
#include "interface.h"
#include "fonctionnalites.h"
#include "admin.h"

// fonction de connexion de l'utilisateur

int login_user(MYSQL *conn, char *compte, char *password, ActiveSession *session) {
    char query[1024];
    MYSQL_RES *result;
    MYSQL_ROW row;

    sprintf(query,
            "SELECT cl.CIN, cl.nom, cl.prenom, cl.n_tele, cl.adresse, ac.solde "
            "FROM accounts AS ac "
            "INNER JOIN client AS cl ON ac.titulaire = cl.CIN "
            "WHERE ac.num_account = '%s' AND ac.mdp = '%s'",
            compte, password);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Erreur Login : %s\n", mysql_error(conn));
        return 0;
    }

    result = mysql_store_result(conn);

    if (result == NULL || mysql_num_rows(result) == 0) {
        mysql_free_result(result);
        return 0;
    }
    row = mysql_fetch_row(result);

    strcpy(session->cin, row[0]);
    strcpy(session->nom, row[1]);
    strcpy(session->prenom, row[2]);
    strcpy(session->n_tele, row[3]);
    strcpy(session->adresse, row[4]);
    session->solde = atof(row[5]);

    strcpy(session->num_account, compte);

    session->is_logged_in = 1;

    mysql_free_result(result);
    return 1;
}

// fonction pour Menu Utilisateur 

void menu_utilisateur(MYSQL *conn, ActiveSession *session) {
    int choix ;
    char compte_dest[17];
    double montant = 0;
    
    printf("\n========================================\n");
    printf("  BIENVENUE %s %s !\n", session->nom, session->prenom);
    printf("========================================\n");
    
    while (1) {
        printf("\n========================================\n");
        printf("        MENU PRINCIPAL\n");
        printf("========================================\n");
        printf("1. Consulter mon compte\n");
        printf("2. Faire un virement\n");
        printf("3. Faire un Depot\n");
        printf("4. Faire un Retrait\n");
        printf("5. Voir mon releve bancaire\n");
        printf("6. Modifier mes informations\n");
        printf("7. Se deconnecter\n");
        printf("----------------------------------------\n");
        printf("Votre choix : ");
        
        if (scanf("%d", &choix) != 1) {
            while(getchar() != '\n');
            printf(" Entree invalide. Veuillez entrer un nombre entre 1 et 7.\n");
            continue;
        }
        while(getchar() != '\n');
        
        switch(choix) {
            case 1: {  
                printf("\n================================================\n");
                printf("        INFORMATIONS DE VOTRE COMPTE\n");
                printf("================================================\n");
                printf("| Nom et Prenom   : %s %s\n", session->nom, session->prenom);
                printf("| Numero de Compte: %s\n", session->num_account);
                printf("| Solde           : %.2f DH\n", session->solde);
                printf("================================================\n\n");
                break;
            }
            
            case 2: {  
                printf("\n--- TRANSACTION ---\n");
                printf("Numero de compte destinataire : ");
                scanf("%16s", compte_dest);
                while(getchar() != '\n');
                
                printf("Montant a transferer : ");
                if (scanf("%lf", &montant) != 1) {
                    while(getchar() != '\n');
                    printf(" Montant invalide.\n");
                    break;
                }
                while(getchar() != '\n');
                
                effectuer_transaction(conn, session, compte_dest, montant);
                break;
            }
            
            case 3: {  
                printf("\n--- DEPOT ---\n");
                printf("Montant a deposer : ");
                if (scanf("%lf", &montant) != 1) {
                    while(getchar() != '\n');
                    printf(" Montant invalide.\n");
                    break;
                }
                while(getchar() != '\n');
                
                effectuer_depot(conn, session, montant);
                break;
            }
            
            case 4: {  
                printf("\n--- RETRAIT ---\n");
                printf("Montant a retirer : ");
                if (scanf("%lf", &montant) != 1) {
                    while(getchar() != '\n');
                    printf(" Montant invalide.\n");
                    break;
                }
                while(getchar() != '\n');
                
                effectuer_retrait(conn, session, montant);
                break;
            }
            
            case 5: {  
                afficher_releve(conn, session->num_account);
                break;
            }
            
            case 6: { 
                modifier_informations(conn, session);
                break;
            }
            
            case 7: {  
                printf("\n========================================\n");
                printf("Au revoir %s !\n", session->prenom);
                printf("Merci d'avoir utilise nos services.\n");
                printf("========================================\n\n");
                session->is_logged_in = 0;
                exit(0);  
                break;
            }
            
            default: {
                printf(" Choix invalide. Veuillez entrer un nombre entre 1 et 7.\n");
                break;
            }
        }
    }
}



// fonction pour Connecter au compte 

void connecter_compte(MYSQL *conn, ActiveSession *session) {
    char input_compte[17];
    char input_mdp[9];
    int tentatives = 0 , choix = 0;
    int connexion_ok = 0;
    char mdp_recupere[9]; // Utilisé au cas de récupération de mot de passe
    
    while (tentatives < 3 && !connexion_ok) {
        printf("\n--- IDENTIFICATION (Tentative %d/3) ---\n", tentatives + 1);
        printf("Numero de compte : ");
        scanf("%16s", input_compte);
        while(getchar() != '\n');
        
        printf("Mot de passe     : ");
        scanf("%8s", input_mdp);
        while(getchar() != '\n');
        
        printf("Verification en cours...\n");
        
        if (login_user(conn, input_compte, input_mdp, session)) {
            printf("\n Connexion reussie.\n");
            connexion_ok = 1;
            menu_utilisateur(conn, session);
        } else {
            tentatives++;
            if (tentatives < 3) {
                printf("\n Identifiants incorrects.\n");
                printf("Il vous reste %d tentative(s).\n\n", 3 - tentatives);
            }
        }
    }
    

    if (!connexion_ok) {
        printf("\n========================================\n");
        printf("   TENTATIVES EPUISEES !\n");
        printf("========================================\n");
        printf("Vous avez oublie votre mot de passe ?\n");
        printf("1. Oui, recuperer mon mot de passe\n");
        printf("2. Non, retour au menu principal\n");
        printf("----------------------------------------\n");
        printf("Votre choix : ");
        
        if (scanf("%d", &choix) != 1) {
            while(getchar() != '\n');
            printf(" Entree invalide.\n");
            return;
        }
        while(getchar() != '\n');
        
        if (choix == 1) {
            if (recuperer_mdp(conn, mdp_recupere)) {
                printf(" Vous pouvez maintenant vous connecter avec ce mot de passe.\n");
            }
        } else if (choix == 2) {
            printf("Retour au menu principal...\n");
            return;
        } else {
            printf("Choix invalide.\n");
        }
    }
}


// Menu administrateur
void menu_admin(MYSQL *conn , ActiveSession *session) {
    int choix ;
    int continuer = 1;
    Queue *q ;
    while (continuer) {
        printf("\n========================================\n");
        printf("   MENU ADMINISTRATEUR\n");
        printf("========================================\n");
        printf("1. Consulter les clients\n");
        printf("2. Ajouter un client\n");
        printf("3. Se deconnecter\n");
        printf("----------------------------------------\n");
        printf("Votre choix : ");
        if (scanf("%d", &choix) != 1) {
            while(getchar() != '\n');
            printf(" Entree invalide.\n");
            continue;
        }
        while(getchar() != '\n');
        
        switch(choix) {
            case 1: {
                q = charger_clients(conn);
                if (q != NULL) {
                    affiche_client(q);
                    liberer_file(q);
                }else{
                    printf(" Aucun client dans la base de donnees.\n");
                }                
                printf("\nAppuyez sur Entree pour continuer...");
                getchar();
                break;
            }
            
            case 2: {
                printf("\n========================================\n");
                printf("   AJOUTER UN CLIENT\n");
                printf("========================================\n");
                Client* C = creer_client();
                client_info(conn, C);
                inserer_client_db(conn, C);
                printf("\nAppuyez sur Entree pour continuer...");
                getchar();
                break;
            }
            
            case 3: {
                printf("\n Deconnexion administrateur.\n");
                continuer = 0;
                break;
            }
            
            default: {
                printf(" Choix invalide. Veuillez entrer un nombre entre 1 et 3.\n");
                break;
            }
        }
    }
}


// fonction pour Menu general 

void menu_general(MYSQL *conn, ActiveSession *session) {
    int choix ;
    char mdp_admin[10];
    char mdp_correct[] = "admin123";  // Mot de passe administrateur par défaut

    printf("==============================================\n\n");
    printf("1. Espace Client\n");
    printf("2. Espace Administrateur\n");
    printf("Votre choix : ");
    scanf("%d",&choix);
    while(getchar() != '\n');
    if(choix == 1){ 
        while (1) {
            printf("\n========================================\n");
            printf("      ACCUEIL BANQUE \n");
            printf("========================================\n");
            printf("1. Se Connecter\n");
            printf("2. Ouvrir Un Compte\n");
            printf("3. Quitter l'application\n");
            printf("----------------------------------------\n");
            printf("Votre choix : ");
            
            if (scanf("%d", &choix) != 1) {
                while(getchar() != '\n');
                printf("Entree invalide. Veuillez entrer un nombre.\n");
                continue;
            }
            while(getchar() != '\n');
            
            switch(choix) {
                case 1: {  
                    connecter_compte(conn, session);
                    break;
                }
                
                case 2: {  
                    creer_compte(conn, session);
                    break;
                }
                
                case 3: { 
                    printf("\n========================================\n");
                    printf("Merci d'avoir choisi notre banque !\n");
                    printf("Au revoir !\n");
                    printf("========================================\n\n");
                    exit(0);
                    break;
                }
                
                default: {
                    printf("Choix invalide. Veuillez entrer un nombre valide (1, 2, 3).\n");
                    break;
                }
            }
        }
    }else if (choix == 2){
        printf("\n========================================\n");
        printf("   CONNEXION ADMINISTRATEUR\n");
        printf("========================================\n");
        printf("Mot de passe administrateur  : ");
        scanf("%9s", mdp_admin);
        if (strcmp(mdp_admin, mdp_correct) == 0) {
            printf("\n Authentification reussie.\n");
            menu_admin(conn,session);
            return;
        }else{
            printf(" Mot de passe incorrect.\n");
            printf("Programme fermer ! \n");

        } 
        }else{
            printf("Programme fermer !\n");
        }

}

