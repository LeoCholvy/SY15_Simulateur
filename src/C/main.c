#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simulator.h"

#define ITERATIONS 100

// Calcul des statistiques pour un tableau de flottants
void calculate_stats_float(float arr[], int n, float *mean, float *stddev) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    *mean = sum / n;
    float variance = 0.0f;
    for (int i = 0; i < n; i++) {
        variance += (arr[i] - *mean) * (arr[i] - *mean);
    }
    *stddev = sqrt(variance / n);
}

// Calcul des statistiques pour un tableau d'entiers
void calculate_stats_int(int arr[], int n, float *mean, float *stddev) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    *mean = sum / n;
    float variance = 0.0f;
    for (int i = 0; i < n; i++) {
        variance += (arr[i] - *mean) * (arr[i] - *mean);
    }
    *stddev = sqrt(variance / n);
}

int main(void){
    float date, temps_attente_agv2;
    int stock_max_warehouse;

    // Tableaux pour sauvegarder les résultats
    float total_time[ITERATIONS];
    float attende_time[ITERATIONS];
    int stock_array[ITERATIONS];

    // Affichage de l'en-tête avec des colonnes de largeur fixe
    printf("%-10s%-20s%-20s%-20s\n", "Itération", "temps total", "temps attente agv2", "stock max warehouse");

    // Exécution de la simulation et sauvegarde dans les tableaux
    for (int i = 0; i < ITERATIONS; i++) {
        Lancer_simulation(&date, &temps_attente_agv2, &stock_max_warehouse);
        total_time[i] = date;
        attende_time[i] = temps_attente_agv2;
        stock_array[i] = stock_max_warehouse;
        printf("%-10d%-20.2f%-20.2f%-20d\n", i + 1, date, temps_attente_agv2, stock_max_warehouse);
    }

    // Calcul des statistiques
    float mean_total, std_total;
    float mean_att, std_att;
    float mean_stock, std_stock;

    calculate_stats_float(total_time, ITERATIONS, &mean_total, &std_total);
    calculate_stats_float(attende_time, ITERATIONS, &mean_att, &std_att);
    calculate_stats_int(stock_array, ITERATIONS, &mean_stock, &std_stock);

    // Affichage des statistiques dans la console
    printf("\nStatistiques:\n");
    printf("%-25s: moyenne = %.2f, ecart type = %.2f\n", "temps total", mean_total, std_total);
    printf("%-25s: moyenne = %.2f, ecart type = %.2f\n", "temps attente agv2", mean_att, std_att);
    printf("%-25s: moyenne = %.2f, ecart type = %.2f\n", "stock max warehouse", mean_stock, std_stock);

    // Génération du fichier CSV
    FILE *fp = fopen("results.csv", "w");
    if (fp == NULL) {
        printf("Erreur d'ouverture du fichier CSV.\n");
        exit(EXIT_FAILURE);
    }
    // En-tête CSV
    fprintf(fp, "Iteration,temps total,temps attente agv2,stock max warehouse\n");
    for (int i = 0; i < ITERATIONS; i++) {
        fprintf(fp, "%d,%.2f,%.2f,%d\n", i + 1, total_time[i], attende_time[i], stock_array[i]);
    }
    // // Statistiques dans le CSV
    // fprintf(fp, "\nStatistiques,,,\n");
    // fprintf(fp, "temps total,%.2f,%.2f\n", mean_total, std_total);
    // fprintf(fp, "temps attente agv2,%.2f,%.2f\n", mean_att, std_att);
    // fprintf(fp, "stock max warehouse,%.2f,%.2f\n", mean_stock, std_stock);
    fclose(fp);

    printf("\nFichier CSV \"results.csv\" genere.\n");

    return 0;
}