#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float date;
    int type; // Type of event
} TypeSchedulerEvent;

#define MAX_EVENTS 9
TypeSchedulerEvent events[MAX_EVENTS];
int nb_events = 0;
#define HORIZON 1000.0f // Simulation horizon

int Scheduler_add(TypeSchedulerEvent event){
    if (event.date > HORIZON) {
        return 1;
    }
    if (nb_events >= MAX_EVENTS) {
        printf("Too many events\n");
        exit(1);
    }
    // Insert the event in the array at the right position
    int i = 0;
    while (i < nb_events && event.date > events[i].date) {
        i++;
    }
    for (int j = nb_events; j > i; j--) {
        events[j] = events[j - 1];
    }
    events[i] = event;
    nb_events++;
    return 0;
}

TypeSchedulerEvent Scheduler_pop_next_event(){
    if (nb_events == 0) {
        printf("No more events\n");
        exit(1);
    }
    TypeSchedulerEvent next_event = events[0];
    for (int i = 1; i < nb_events; i++) {
        events[i - 1] = events[i];
    }
    nb_events--;
    return next_event;
}

int Scheduler_get_nb_events(){
    return nb_events;
}

int Scheduler_reset(){
    int nb_events = 0;
    return 0;
}
