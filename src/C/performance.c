#include <string.h>
#include "simulator.c"

static double total_waiting_time = 0.0;
static int    nb_wait_events = 0;
static double wait_start = 0.0;


// TODO: juste un EXEMPLE :
void metrics_listener(const char *var, int old_v, int new_v) {
    if (strcmp(var, "attente.etat")==0 && old_v==EV_DISPO && new_v==EV_OCCUPE) {
        wait_start = get_current_time();
    }
    if (strcmp(var, "attente.etat")==0 && old_v==EV_OCCUPE && new_v!=EV_OCCUPE) {
        total_waiting_time += get_current_time() - wait_start;
        nb_wait_events++;
    }
}

double get_total_wait() { return total_waiting_time; }
int    get_nb_wait_events() { return nb_wait_events; }