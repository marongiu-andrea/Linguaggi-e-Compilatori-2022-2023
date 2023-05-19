#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

extern void populate (int a[], int b[], int c[]);

#define N 100
#define OOMS 10
#define BASE 10


void main() {
    
    FILE *fp = fopen("loopopt.txt", "w");

    if (fp == NULL)
        return;

    int a[N], b[N], c[N];

    for (int i = 0; i < N; ++i) {
        a[i] = 1;
    }

    for (int i = 0; i < OOMS; ++i) {
        int num_cycles = pow(BASE, i);

        clock_t start = clock();

        for (int j = 0; j < num_cycles; ++j) {
            populate(a, b, c);
        }

        clock_t stop = clock();

        double time = (stop - start) / (double) CLOCKS_PER_SEC;
        printf("N° loops: %d, Exec time: %f \n", num_cycles, time);
        fprintf(fp, "%f\t", time);
    }    
}