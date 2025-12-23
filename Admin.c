#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "admin.h"
#include "acreation.h"
#include "interface.h"


Queue* creer_file() {
    Queue *q = (Queue*)malloc(sizeof(Queue));
    if (q == NULL) {
        printf("Un probleme dans le systeme , recommencer plutard.\n"); // probléme allocation
        return NULL;
    }
    q->debut = NULL;
    q->fin = NULL;
    return q;
}

void enfiler(Queue *q, ClientInfo data) {
    Node *new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Un probleme dans le systeme , recommencer plutard\n");// probléme allocation
        return;
    }
    

    new_node->data = data;
    new_node->next = NULL;
    
    if (q->fin == NULL) {
        q->debut = new_node;
        q->fin = new_node;
    } else {
        q->fin->next = new_node;
        q->fin = new_node;
    }
}


ClientInfo defiler(Queue *q) {
    ClientInfo vide = {0};
    
    if (verif_vide(q)) {  
        return vide;
    }
    
    Node *temp = q->debut;
    ClientInfo data = temp->data;
    q->debut = q->debut->next;
    
    if (q->debut == NULL) {
        q->fin = NULL;
    }
    
    free(temp);
    return data;
}


int verif_vide(Queue *q) {
    return (q->debut == NULL);
}

// Libérer toute la mémoire de la file
void liberer_file(Queue *q) {
    if (q == NULL) return;
    
    Node *lib = q->debut;
    while (lib != NULL) {
        Node *temp = lib;
        lib = lib->next;
        free(temp);
    }
    free(q);
}


// fonction pour afficher tous les clients 
void affiche_client(Queue *q) {
    Node *client = q->debut;
    
    printf("\n");
    printf("=========================================================================================\n");
    printf("                       LISTE DES CLIENTS\n");
    printf("=========================================================================================\n");
    printf("%-20s %-20s %-15s %-18s %s\n", 
           "NOM", "PRENOM", "CIN", "NM COMPTE", " SOLDE ");
    printf("=========================================================================================\n");

    while (client != NULL) {
        printf("%-20s %-20s %-15s %-18s %9.2f DH\n",
               client->data.nom,
               client->data.prenom,
               client->data.cin,
               client->data.num_compte,
               client->data.solde);
        client = client->next;
    }
    
}


// Charger tous les clients dans une file

Queue* charger_clients(MYSQL *conn) {
    char query[1024];
    MYSQL_RES *result;
    MYSQL_ROW row;   
    Queue *q = creer_file();

    if (q == NULL) {
        return NULL;
    }
    
    sprintf(query,
            "SELECT cl.nom, cl.prenom, cl.CIN, ac.num_account, ac.solde "
            "FROM client AS cl "
            "INNER JOIN accounts AS ac ON cl.CIN = ac.titulaire "
            "ORDER BY cl.nom, cl.prenom");
    
    if (mysql_query(conn, query)) {
        printf(" Un probleme dans le systeme !\n");
        liberer_file(q);
        return NULL;
    }
    
    result = mysql_store_result(conn);
    
    if (result == NULL) {
        printf("  Un probleme dans le systeme ! \n");
        liberer_file(q);
        return NULL;
    }
    
    // Ajouter chaque client à la file
    while ((row = mysql_fetch_row(result)) != NULL) {
        ClientInfo client;
        strcpy(client.nom, row[0] ? row[0] : "");
        strcpy(client.prenom, row[1] ? row[1] : "");
        strcpy(client.cin, row[2] ? row[2] : "");
        strcpy(client.num_compte, row[3] ? row[3] : "N/A");
        client.solde = row[4] ? atof(row[4]) : 0.0;
        
        enfiler(q, client);
    }
    
    mysql_free_result(result);
    return q;
}
