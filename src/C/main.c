#include <stdio.h>
#include "simulator.h"

int main(void){
    float date;
    Lancer_simulation(&date);
    printf("Final date: %.2f\n", date);
    return 0;
}