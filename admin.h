#ifndef ADMIN_H
#define ADMIN_H

#include <mysql.h>
#include "interface.h"

// Structure pour représenter un client dans la file
typedef struct {
    char nom[50];
    char prenom[50];
    char cin[20];
    char num_compte[17];
    double solde;
} ClientInfo;

// Structure pour un nœud de la file
typedef struct Node {
    ClientInfo data;
    struct Node *next;
} Node;

// Structure pour la file 
typedef struct {
    Node *debut;  
    Node *fin;   
} Queue; 


Queue* creer_file();
void enfiler(Queue *q, ClientInfo data);
ClientInfo defiler(Queue *q);
int verif_vide(Queue *q);
void liberer_file(Queue *q);

void affiche_client(Queue *q);
Queue* charger_clients(MYSQL *conn);



#endif
