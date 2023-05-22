#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

extern void populate (int a[], int b[], int c[], int N, int S);
// N -> # loops, S -> stride

#define OOMS 17
#define BASE 2
#define LOOPS 10000
#define SIZE 4


void main() {
    FILE *fp = fopen("cache_exp.txt", "w");

    for (int i = 0; i < OOMS; ++i) {
        int stride = pow(BASE, i);
        int size = SIZE * stride;
        int a[size], b[size], c[size];

        clock_t start = clock();

        for (int j = 0; j < LOOPS; ++j) {
            populate(a, b, c, size, stride);
        }

        clock_t stop = clock();

        double time = (stop - start) / (double) CLOCKS_PER_SEC;
        printf("Stride: %d \t Array size: %d \t Exec time: %f \n", stride, size, time);
        fprintf(fp, "%d\t%f\n", stride, time);
    }

}