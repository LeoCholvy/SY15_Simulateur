#include <stdio.h>
#include "randgen.h"
#include "scheduler.h"

typedef struct {
    float mean; // Mean value of the parameter
    float stddev; // Standard deviation of the parameter
} TypeNormalParameter;

// Files
typedef enum {
    FIFO,
    LIFO
} TypeFile;



// ### MODEL (Parameter) declarations
#define nb_commandes_totale 64 // Number of commands
#define capa_stockage 64
// ### MODEL (Variable) declarations
// Servers
typedef enum {
    AGV_ETAT_DISPONIBLE, // Available
    AGV_ETAT_EN_CHARGEMENT, // Loading
    AGV_ETAT_EN_DECHARGEMENT, // Unloading
    AGV_ETAT_EN_ROUTE, // In transit
    AGV_ETAT_EN_PAUSE // Paused
} TypeAGVEtat;
typedef struct {
    TypeNormalParameter repos_warehouse;
    TypeNormalParameter prod1_warehouse;
    TypeNormalParameter warehouse_repos;
    TypeNormalParameter chargement;
    TypeNormalParameter dechargement;
    int num_commande; // -1 : no command, >=0 : command number
    TypeAGVEtat etat; // State of the AGV
} AGV;
AGV agv1;
AGV agv2;
// Clients
int commandes[nb_commandes_totale];
// Ressources
int ressource_zone_attente; // 0: no AGV in front of the warehouse, 1: AGV1 in front of the warehouse, 2: AGV2 in front of the production line
// Zones d'attentes
typedef struct TypeZoneAttente {
    int commandes[capa_stockage];
    int nb_commandes; // Number of commands in the waiting area
    TypeFile type;
} TypeZoneAttente;
TypeZoneAttente prod1, client2, warehouse;
int file_ajouter(int num_commande, TypeZoneAttente zone){
    if (zone.type == FIFO) {
        printf("coucou FIFO\n");
    }
};
int file_retirer(TypeZoneAttente zone){

};



// ### EVENTS declarations
enum {
    ARRIVEE_AGV1_PROD1 = 0,
    FIN_CHARGEMENT_AGV1_PROD1 = 1,
    ARRIVEE_AGV1_WAREHOUSE = 2,
    FIN_DECHARGEMENT_AGV1_WAREHOUSE = 3,
    ARRIVEE_AGV2_WAREHOUSE = 4,
    FIN_CHARGEMENT_AGV2_WAREHOUSE = 5,
    ARRIVEE_AGV2_CLIENT2 = 6,
    FIN_DECHARGEMENT_AGV2_CLIENT2 = 7,
    ARRIVEE_AGV2_REPOS = 8,
} TypeEvent;


// ### SIMULATION Parameters
int Init_simulation(){
    Scheduler_reset();
    TypeSchedulerEvent event = {0.0f, ARRIVEE_AGV1_PROD1};
    Scheduler_add(event);
    event.date = Exp(10);
    event.type = FIN_CHARGEMENT_AGV1_PROD1;
    Scheduler_add(event);
}


int Lancer_simulation(float *date){
    Init_simulation();
    *date = 0.0f;

    while (Scheduler_get_nb_events() > 0) {
        TypeSchedulerEvent event = Scheduler_pop_next_event();
        *date = event.date;

        switch (event.type) {
            case ARRIVEE_AGV1_PROD1:
                printf("AGV1 arrived at production line at time %.2f\n", *date);
                // Handle AGV1 arrival at production line
                break;
            case FIN_CHARGEMENT_AGV1_PROD1:
                printf("AGV1 finished loading at production line at time %.2f\n", *date);
                // Handle AGV1 finishing loading
                break;
            case ARRIVEE_AGV1_WAREHOUSE:
                printf("AGV1 arrived at warehouse at time %.2f\n", *date);
                // Handle AGV1 arrival at warehouse
                break;
            case FIN_DECHARGEMENT_AGV1_WAREHOUSE:
                printf("AGV1 finished unloading at warehouse at time %.2f\n", *date);
                // Handle AGV1 finishing unloading
                break;
            case ARRIVEE_AGV2_WAREHOUSE:
                printf("AGV2 arrived at warehouse at time %.2f\n", *date);
                // Handle AGV2 arrival at warehouse
                break;
            case FIN_CHARGEMENT_AGV2_WAREHOUSE:
                printf("AGV2 finished loading at warehouse at time %.2f\n", *date);
                // Handle AGV2 finishing loading
                break;
            case ARRIVEE_AGV2_CLIENT2:
                printf("AGV2 arrived at client 2 at time %.2f\n", *date);
                // Handle AGV2 arrival at client 2
                break;
            case FIN_DECHARGEMENT_AGV2_CLIENT2:
                printf("AGV2 finished unloading at client 2 at time %.2f\n", *date);
                // Handle AGV2 finishing unloading
                break;
            case ARRIVEE_AGV2_REPOS:
                printf("AGV2 returned to rest area at time %.2f\n", *date);
                // Handle AGV2 returning to rest area
                break;
            default:
                printf("Unknown event type %d at time %.2f\n", event.type, *date);
        }
    }

    return 0;
}