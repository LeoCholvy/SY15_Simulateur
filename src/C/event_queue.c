#include <stdlib.h>
#include <stdio.h>

typedef struct event {
    double           date;
    void           (*callback)(void*);
    void            *arg;
    struct event    *next;
} Event;

static Event *events = NULL;

Event* new_event(double date, void (*cb)(void*), void *arg) {
    Event *e = malloc(sizeof(Event));
    if (!e) { perror("malloc"); exit(EXIT_FAILURE); }
    e->date = date;
    e->callback = cb;
    e->arg = arg;
    e->next = NULL;
    return e;
}

void schedule_event(Event *e) {
    if (!events || e->date < events->date) {
        e->next = events;
        events = e;
        return;
    }
    Event *p = events;
    while (p->next && p->next->date <= e->date) p = p->next;
    e->next = p->next;
    p->next = e;
}

Event* pop_next_event(void) {
    if (!events) return NULL;
    Event *e = events;
    events = e->next;
    e->next = NULL;
    return e;
}

void clear_events(void) {
    Event *cur = events;
    while (cur) {
        Event *tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    events = NULL;
}