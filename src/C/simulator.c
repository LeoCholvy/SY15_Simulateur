//int simulationUnique() {
//    // initialisation des variables globales
//
//    // traiter chaque événements
//    while (!isEmpty(events)) {
//        // récupérer le prochaine event
//        // changer la date
//        // traiter l'event
//    }
//
//    return 0;
//}

#include "modele.c"
#include "event_queue.c"
#include "event.c"

double date = 0.0;

double get_current_time(void) {
    return date;
}

void run_simulation(void) {
    Event *ev;
    while ((ev = pop_next_event())) {
        date = ev->date;
        ev->callback(ev->arg);
        free(ev);
    }
}