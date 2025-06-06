#include <signal.h>
#include <stdio.h>
#include "randgen.h"
#include "scheduler.h"

typedef struct {
    float mean; // Mean value of the parameter
    float stddev; // Standard deviation of the parameter
} TypeNormalParameter;

// Files
typedef enum {
    FIFO = 1,
    LIFO = 2,
    RANDOM = 3,
    SMALLEST = 4,
    BIGGEST = 5,
} TypeFile;



// ### MODEL (Parameter) declarations
#define nb_commandes_totale 64 // Number of commands
#define capa_stockage 64
float t;
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
    TypeNormalParameter repos_prod1;
    TypeNormalParameter prod1_warehouse;
    TypeNormalParameter warehouse_repos;

    TypeNormalParameter repos_warehouse;
    TypeNormalParameter warehouse_client2;
    TypeNormalParameter client2_repos;

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
    int type;
} TypeZoneAttente;
TypeZoneAttente prod1, client2, warehouse;
int check_max_stock_warehouse();
typedef enum TypeEnumZonesAttente {
    zone_prod1 = 0,
    zone_client2 = 1,
    zone_warehouse = 2,
};

int file_ajouter(int num_commande, int z){
    // chosir la bonne zone de stockage en fonction de z
    TypeZoneAttente *zone;
    switch (z) {
        case zone_prod1:
            zone = &prod1;
            break;
        case zone_client2:
            zone = &client2;
            break;
        case zone_warehouse:
            zone = &warehouse;
            break;
        default:
            printf("Error: Unknown waiting area type %d\n", z);
            exit(-1);
    }

    // Ajouter la commande
    int n = zone->nb_commandes;
    if (n < capa_stockage) {
        zone->commandes[n] = num_commande;
        zone->nb_commandes++;
        if (z == zone_warehouse) {
            check_max_stock_warehouse();
            // printf("nb dans warehouse: %d\n", warehouse.nb_commandes);
        }
        return 0; // Succès
    } else {
        printf("Error: Waiting area %d is full\ndate: %f", z, t);
        exit(-1);
    }
};

int file_retirer(int z){
    // Choisir la bonne zone de stockage en fonction de z
    TypeZoneAttente *zone;
    switch (z) {
        case zone_prod1:
            zone = &prod1;
            break;
        case zone_client2:
            zone = &client2;
            break;
        case zone_warehouse:
            zone = &warehouse;
            break;
        default:
            printf("Error: Unknown waiting area type %d\n", z);
            exit(-1);
    }

    // Retirer une commande en fonction de la discipline de service de la zone
    if (zone->nb_commandes > 0) {
        switch (zone->type) {
            case FIFO: {
                int num_commande = zone->commandes[0];
                for (int i = 0; i < zone->nb_commandes - 1; i++) {
                    zone->commandes[i] = zone->commandes[i + 1];
                }
                zone->nb_commandes--;
                return num_commande; // Retourne la première commande
            }
            case LIFO: {
                int num_commande = zone->commandes[zone->nb_commandes - 1];
                zone->nb_commandes--;
                return num_commande; // Retourne la dernière commande
            }
            case RANDOM: {
                int idx = RandInt(0, zone->nb_commandes - 1); // Sélectionne un index aléatoire
                int num_commande = zone->commandes[idx];
                for (int i = idx; i < zone->nb_commandes - 1; i++) {
                    zone->commandes[i] = zone->commandes[i + 1];
                }
                zone->nb_commandes--;
                return num_commande;
            }
            case SMALLEST: {
                // trouver le plus petit
                int min_idx;
                min_idx = 0;
                for (int i=0; i<zone->nb_commandes; i++) {
                    if (commandes[zone->commandes[i]] < min_idx) {
                        min_idx = i;
                    }
                }
                int num = commandes[zone->commandes[min_idx]];
                // retirer le plus petit
                for (int i = min_idx; i < zone->nb_commandes; i++) {
                    zone->commandes[i] = zone->commandes[i + 1];
                }
                zone->nb_commandes --;
                return num;
            }
            case BIGGEST: {
                // trouver le plus grand
                int max_idx;
                max_idx = 0;
                for (int i=0; i<zone->nb_commandes; i++) {
                    if (zone->commandes[i] > max_idx) {
                        max_idx = i;
                    }
                }
                int num = zone->commandes[max_idx];
                // retirer le plus grand
                for (int i = max_idx; i < zone->nb_commandes; i++) {
                    zone->commandes[i] = zone->commandes[i + 1];
                }
                zone->nb_commandes--;
                return num;
            }
            default:
                printf("Error: Unknown file type %d\n", zone->type);
                exit(-1);
        }
    }
    return -1;
};

// ### Metrics declarations
float temps_attente_agv2;
float derniere_date_agv2_debut_dispo;
int stock_max_warehouse;
int check_max_stock_warehouse(){
    if (warehouse.nb_commandes > stock_max_warehouse) {
        stock_max_warehouse = warehouse.nb_commandes;
    }
    return 0;
}
int set_etat_agv2(TypeAGVEtat etat){
    if (agv2.etat == etat) {
        return 0;
    } else {
        if (etat == AGV_ETAT_DISPONIBLE) {
            derniere_date_agv2_debut_dispo = t;
        } else if (etat == AGV_ETAT_EN_ROUTE && agv2.etat == AGV_ETAT_DISPONIBLE) {
            temps_attente_agv2 += t - derniere_date_agv2_debut_dispo;
        }
    }

    agv2.etat = etat;
    return 0;
}


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
int Arrivee_AGV1_Prod1();
int Fin_Chargement_AGV1_Prod1();
int Arrivee_AGV1_Warehouse();
int Fin_Dechargement_AGV1_Warehouse();
int Arrivee_AGV2_Warehouse();
int Fin_Chargement_AGV2_Warehouse();
int Arrivee_AGV2_Client2();
int Fin_Dechargement_AGV2_Client2();
int Arrivee_AGV2_Repos();
// ### EVENTS Functions
int Arrivee_AGV1_Prod1() {
    int num_commande = file_retirer(zone_prod1);
    // Prévoir date fin chargement
    agv1.etat = AGV_ETAT_EN_CHARGEMENT;
    float a,b;
    a = agv1.chargement.mean;
    b = agv1.chargement.stddev;
    TypeSchedulerEvent event ={
        t + N(a, b) * commandes[num_commande],
        FIN_CHARGEMENT_AGV1_PROD1
    };
    agv1.num_commande = num_commande;
    Scheduler_add(event);
}
int Fin_Chargement_AGV1_Prod1() {
    agv1.etat = AGV_ETAT_EN_ROUTE;
    float a,b;
    a = agv1.prod1_warehouse.mean;
    b = agv1.prod1_warehouse.stddev;
    TypeSchedulerEvent event = {
        t + N(a, b),
        ARRIVEE_AGV1_WAREHOUSE
    };
    Scheduler_add(event);
}
int Arrivee_AGV1_Warehouse() {
    if (ressource_zone_attente == 0) {
        ressource_zone_attente = 1;
        agv1.etat = AGV_ETAT_EN_DECHARGEMENT;
        float a, b;
        a = agv1.dechargement.mean;
        b = agv1.dechargement.stddev;
        TypeSchedulerEvent event = {
            t + N(a, b) * commandes[agv1.num_commande],
            FIN_DECHARGEMENT_AGV1_WAREHOUSE
        };
        Scheduler_add(event);
    } else {
        agv1.etat = AGV_ETAT_EN_PAUSE;
    }
}
int Fin_Dechargement_AGV1_Warehouse() {
    file_ajouter(agv1.num_commande, zone_warehouse);
    agv1.num_commande = -1; // Reset command number
    ressource_zone_attente = 0; // Release the resource

    agv1.etat = AGV_ETAT_EN_ROUTE;
    if (prod1.nb_commandes > 0) {
        float a, b;
        a = agv1.prod1_warehouse.mean;
        b = agv1.prod1_warehouse.stddev;
        TypeSchedulerEvent event ={
            t + N(a,b),
            ARRIVEE_AGV1_PROD1,
        };
        Scheduler_add(event);
    } else {
        // On ne fait plus rien AGV1 a fini son travail
    }

    if (agv2.etat == AGV_ETAT_EN_PAUSE) {
        // Arrivée immediate de l'AGV2 à warehouse
        TypeSchedulerEvent event ={
            t,
            ARRIVEE_AGV2_WAREHOUSE,
        };
        Scheduler_add(event);
    } else if (agv2.etat == AGV_ETAT_DISPONIBLE) {
        // faire venir AGV2 a warehouse
        // agv2.etat = AGV_ETAT_EN_ROUTE;
        set_etat_agv2(AGV_ETAT_EN_ROUTE);
        float a, b;
        a = agv2.repos_warehouse.mean;
        b = agv2.repos_warehouse.stddev;
        TypeSchedulerEvent event ={
            t + N(a,b),
            ARRIVEE_AGV2_WAREHOUSE,
        };
        Scheduler_add(event);
    }
}
int Arrivee_AGV2_Warehouse(){
    if (ressource_zone_attente == 0) {
        ressource_zone_attente = 2;

        // agv2.etat = AGV_ETAT_EN_CHARGEMENT;
        set_etat_agv2(AGV_ETAT_EN_CHARGEMENT);
        int num_commandes = file_retirer(zone_warehouse);
        agv2.num_commande = num_commandes;
        float a, b;
        a = agv2.chargement.mean;
        b = agv2.chargement.stddev;
        TypeSchedulerEvent event ={
            t + N(a,b) * commandes[num_commandes],
            FIN_CHARGEMENT_AGV2_WAREHOUSE,
        };
        Scheduler_add(event);
    } else {
        // agv2.etat = AGV_ETAT_EN_PAUSE;
        set_etat_agv2(AGV_ETAT_EN_PAUSE);
    }
}
int Fin_Chargement_AGV2_Warehouse(){
    ressource_zone_attente = 0;
    set_etat_agv2(AGV_ETAT_EN_ROUTE);
    float a, b;
    a = agv2.warehouse_client2.mean;
    b = agv2.warehouse_client2.stddev;
    TypeSchedulerEvent event ={
        t + N(a,b),
        ARRIVEE_AGV2_CLIENT2,
    };
    Scheduler_add(event);

    if (agv1.etat == AGV_ETAT_EN_PAUSE) {
        // arrivée imminente agv1 warehouse
        TypeSchedulerEvent event ={
            t,
            ARRIVEE_AGV1_WAREHOUSE,
        };
        Scheduler_add(event);
    }
}
int Arrivee_AGV2_Client2(){
    // agv2.etat = AGV_ETAT_EN_DECHARGEMENT;
    set_etat_agv2(AGV_ETAT_EN_DECHARGEMENT);
    float a, b;
    a = agv2.dechargement.mean;
    b = agv2.dechargement.stddev;
    TypeSchedulerEvent event ={
        t + N(a,b) * commandes[agv2.num_commande],
        FIN_DECHARGEMENT_AGV2_CLIENT2,
    };
    Scheduler_add(event);
}
int Fin_Dechargement_AGV2_Client2(){
    file_ajouter(agv2.num_commande, zone_client2);
    agv2.num_commande = -1;
    if (warehouse.nb_commandes > 0) {
        // aller vers warehouse
        // agv2.etat = AGV_ETAT_EN_ROUTE;
        set_etat_agv2(AGV_ETAT_EN_ROUTE);
        float a, b;
        a = agv2.warehouse_client2.mean;
        b = agv2.warehouse_client2.stddev;
        TypeSchedulerEvent event ={
            t + N(a,b),
            ARRIVEE_AGV2_WAREHOUSE,
        };
        Scheduler_add(event);
    } else {
        // aller vers repos
        set_etat_agv2(AGV_ETAT_EN_ROUTE);
        float a, b;
        a = agv2.client2_repos.mean;
        b = agv2.client2_repos.stddev;
        TypeSchedulerEvent event ={
            t + N(a,b),
            ARRIVEE_AGV2_REPOS,
        };
        Scheduler_add(event);
    }
}
int Arrivee_AGV2_Repos(){
    if (warehouse.nb_commandes > 0) {
        // aller vers warehouse
        set_etat_agv2(AGV_ETAT_EN_ROUTE);
        float a, b;
        a = agv2.repos_warehouse.mean;
        b = agv2.repos_warehouse.stddev;
        TypeSchedulerEvent event ={
            t + N(a,b),
            ARRIVEE_AGV2_WAREHOUSE,
        };
        Scheduler_add(event);
    } else {
        set_etat_agv2(AGV_ETAT_DISPONIBLE);
    }
}



// ### SIMULATION Parameters
int Init_simulation(){
    agv1.num_commande = -1;
    agv1.etat = AGV_ETAT_EN_ROUTE;
    agv2.num_commande = -1;
    agv2.etat = AGV_ETAT_DISPONIBLE;
    agv1.chargement.mean = 17.96f; agv2.chargement.mean = 17.96f;
    agv1.chargement.stddev = 0.93; agv2.chargement.stddev = 0.93;
    agv1.dechargement.mean = 17.97; agv2.dechargement.mean = 17.97;
    agv1.dechargement.mean = 0.71f; agv2.dechargement.mean = 0.71f;
    agv1.repos_prod1.mean = 26.58f;
    agv1.repos_prod1.stddev = 0.67;
    agv1.prod1_warehouse.mean = 25.96f;
    agv1.prod1_warehouse.stddev = 3.18f;
    agv1.warehouse_repos.mean = 18.34f;
    agv1.warehouse_repos.stddev = 2.20f;
    agv2.repos_warehouse.mean = 28.27f;
    agv2.repos_warehouse.stddev = 1.62f;
    agv2.warehouse_client2.mean = 30.04f;
    agv2.warehouse_client2.stddev = 1.75f;
    agv2.client2_repos.mean = 21.58f;
    agv2.client2_repos.stddev = 1.22f;

    ressource_zone_attente = 0;

    prod1.type = FIFO;
    prod1.nb_commandes = 0;
    warehouse.type = FIFO;
    warehouse.nb_commandes = 0;
    client2.type = FIFO; // ne change rien
    client2.nb_commandes = 0;

    // commandes
    int somme = 0, i = 0, r;
    while (nb_commandes_totale - somme > 6) {
        r = RandInt(1, 6);
        commandes[i] = r;
        file_ajouter(i, zone_prod1);
        somme += r;
        i ++;
    }

    // metrics
    temps_attente_agv2 = 0.0f;
    stock_max_warehouse = 0;
    derniere_date_agv2_debut_dispo = 0.0f;

    // Scheduler
    Scheduler_reset();
    TypeSchedulerEvent event = {0.0f, ARRIVEE_AGV1_PROD1};
    Scheduler_add(event);

    // Others
    t = 0.0f;
}

int NBSNDBN = 0;

int Lancer_simulation(float *p_date, float *p_temps_attente_agv2, int *p_stock_max_warehouse) {
    Init_simulation();

    while (Scheduler_get_nb_events() > 0) {
        TypeSchedulerEvent event = Scheduler_pop_next_event();
        t = event.date;
        // printf("Date: %.2f\n", event.date);
        // printf("Nombres events prévus : %d\n", Scheduler_get_nb_events());

        switch (event.type) {
            case ARRIVEE_AGV1_PROD1:
                // printf("ARRIVEE_AGV1_PROD1\n");
                Arrivee_AGV1_Prod1();
                // printf("Nombres commandes dans Prod1: %d\n", prod1.nb_commandes);
        NBSNDBN ++;
                break;
            case FIN_CHARGEMENT_AGV1_PROD1:
                // printf("FIN_CHARGEMENT_AGV1_PROD1\n");
                Fin_Chargement_AGV1_Prod1();
                break;
            case ARRIVEE_AGV1_WAREHOUSE:
                // printf("ARRIVEE_AGV1_WAREHOUSE\n");
                Arrivee_AGV1_Warehouse();
                break;
            case FIN_DECHARGEMENT_AGV1_WAREHOUSE:
                // printf("FIN_DECHARGEMENT_AGV1_WAREHOUSE\n");
                Fin_Dechargement_AGV1_Warehouse();
                break;
            case ARRIVEE_AGV2_WAREHOUSE:
                // printf("ARRIVEE_AGV2_WAREHOUSE\n");
                Arrivee_AGV2_Warehouse();
                break;
            case FIN_CHARGEMENT_AGV2_WAREHOUSE:
                // printf("FIN_CHARGEMENT_AGV2_WAREHOUSE\n");
                Fin_Chargement_AGV2_Warehouse();
                break;
            case ARRIVEE_AGV2_CLIENT2:
                // printf("ARRIVEE_AGV2_CLIENT2\n");
                Arrivee_AGV2_Client2();
                break;
            case FIN_DECHARGEMENT_AGV2_CLIENT2:
                // printf("FIN_DECHARGEMENT_AGV2_CLIENT2\n");
                Fin_Dechargement_AGV2_Client2();
                break;
            case ARRIVEE_AGV2_REPOS:
                // printf("ARRIVEE_AGV2_REPOS\n");
                Arrivee_AGV2_Repos();
                break;
            default:
                printf("Unknown event type %d at time %.2f\n", event.type, *p_date);
        }

        *p_date = t;
        *p_temps_attente_agv2 = temps_attente_agv2;
        *p_stock_max_warehouse = stock_max_warehouse;
    }
    return 0;
}