#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

extern void populate (int a[], int b[], int c[]);

#define M 1000000

void main() {
    clock_t start = clock();

    int a[100], b[100], c[100];

    for (int i = 0; i < 100; i++) {
        a[i] = 1;
    }

    for (int i = 0; i < M; i++) {
        populate(a, b, c);
    }

    clock_t stop = clock();

    printf("Exec time: %f \n", (stop - start) / (double) CLOCKS_PER_SEC);
}