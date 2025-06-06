#include <stdio.h>
#include "simulator.h"

int main(void){
    float date;
    float temps_attente_agv2;
    int stock_max_warehouse;
    Lancer_simulation(&date, &temps_attente_agv2, &stock_max_warehouse);
    printf("Final date: %.2f\n", date);
    printf("temps_attente_agv2: %.2f\n", temps_attente_agv2);
    printf("stock_max_warehouse: %d\n", stock_max_warehouse);
    return 0;
}