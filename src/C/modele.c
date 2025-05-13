#include <stdlib.h>
#include <string.h>

/* === Types et variables globales === */

typedef enum {
    STATE_REPOS,
    STATE_PROD1_LOAD,
    STATE_PROD1_MOVE,
    STATE_WAREHOUSE_LOAD,
    STATE_WAREHOUSE_MOVE,
    STATE_CLIENT2_LOAD,
    STATE_CLIENT2_MOVE,
    STATE_WAIT
} agv_state_t;

typedef enum {
    POS_PROD1,
    POS_WAREHOUSE,
    POS_CLIENT2,
    POS_REPOS
} position_t;

typedef struct commande {
    int                 id;
    int                 nb_produits;
    position_t          position;
    struct commande    *next;
} commande_t;

typedef struct {
    commande_t *head;
    commande_t *tail;
    int         size;
} queue_t;

typedef struct {
    agv_state_t etat;
    int         current_cmd_id;
} agv_t;

typedef enum { EV_DISPO, EV_OCCUPE } attente_state_t;

/* Globals */
agv_t AGV1 = { STATE_REPOS, -1 };
agv_t AGV2 = { STATE_REPOS, -1 };
attente_state_t attente = EV_DISPO;
queue_t Prod1_queue = { NULL, NULL, 0 };
queue_t Warehouse_queue = { NULL, NULL, 0 };

/* Enqueue/Dequeue */
void enqueue(queue_t *q, commande_t *cmd) {
    cmd->next = NULL;
    if (q->tail) q->tail->next = cmd;
    else q->head = cmd;
    q->tail = cmd;
    q->size++;
}

commande_t* dequeue(queue_t *q) {
    if (!q->head) return NULL;
    commande_t* cmd = q->head;
    q->head = cmd->next;
    if (!q->head) q->tail = NULL;
    cmd->next = NULL;
    q->size--;
    return cmd;
}

/* Observer */

typedef void (*callback_t)(const char *var, int old_v, int new_v);

#define MAX_LISTENERS 32
static callback_t listeners[MAX_LISTENERS];
static int nb_listeners = 0;

void subscribe(callback_t cb) {
    if (nb_listeners < MAX_LISTENERS)
        listeners[nb_listeners++] = cb;
}

void notify_state_change(const char *var, int old_v, int new_v) {
    for (int i = 0; i < nb_listeners; i++)
        listeners[i](var, old_v, new_v);
}

void set_agv_state(agv_t* agv, const char* name, agv_state_t new_state) {
    int old = agv->etat;
    agv->etat = new_state;
    notify_state_change(name, old, new_state);
}