#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

extern void populate (int a[], int b[], int c[]);

#define M 1000000

void main() {
    struct timespec start, finish;

    clock_gettime(CLOCK_REALTIME, &start);

    int a[100], b[100], c[100];

    for (int i = 0; i < 100; i++) {
        a[i] = 1;
    }

    for (int i = 0; i < M; i++) {
        populate(a, b, c);
    }

    clock_gettime(CLOCK_REALTIME, &finish);

    printf("%f\n", ((double) (finish.tv_nsec - start.tv_nsec))/((double) 1000000));
}