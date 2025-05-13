#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Paramètres du générateur
#define A 1664525
#define B 1013904223
#define C 4294967296

// Graine initiale
unsigned long long X = 123456789;

double U01() {
    X = (A * X + B) % C;
    return (double)X / C;
}

double U(double a, double b) {
    return a + U01() * (b - a);
}

double exp(double l) {
    return (double)-1.0/l * log(U01());
}

double N01() {
    int n, i;
    double r, somme;

    n = 100;
    somme = 0;
    for (i = 0; i < n; i++) {
        r = U01();
        somme += r;
    }
    return sqrt(12)/sqrt(n) * (somme - n/2);
}

double N(double n, double s) {
    return N01() * s + n;
}