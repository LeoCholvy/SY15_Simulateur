#include "modele.c"
#include "event_queue.c"
#include <stdlib.h>

void schedule_initial_events(void) {
    /* Planifier arrivée de la 1ère commande chez Prod1, etc. */
}

/* Callbacks (9) : AGV1_arrive_prod1, AGV1_load_end, AGV1_arrive_warehouse,
                    AGV1_unload_end, AGV1_arrive_repos,
                    AGV2_arrive_warehouse, AGV2_load_end,
                    AGV2_arrive_client2, AGV2_unload_end */

void AGV1_arrive_prod1(void *arg) {
    /* ... */
}

void AGV1_load_end(void *arg) {
    /* ... */
}

void AGV1_arrive_warehouse(void *arg) {
    /* ... */
}

void AGV1_unload_end(void *arg) {
    /* ... */
}

void AGV1_arrive_repos(void *arg) {
    /* ... */
}

void AGV2_arrive_warehouse(void *arg) {
    /* ... */
}

void AGV2_load_end(void *arg) {
    /* ... */
}

void AGV2_arrive_client2(void *arg) {
    /* ... */
}

void AGV2_unload_end(void *arg) {
    /* ... */
}
