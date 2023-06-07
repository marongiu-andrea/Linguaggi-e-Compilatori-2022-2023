#define N 40000
#define BILLION 1000000000L
#include <stdlib.h>
#include <stdio.h>
#include "time.h"

void populate(int a[N], int b[N], int c[N]) {
    int i;
    for(i=0; i<N; i++) {
        a[i] = 5*c[N-i-1];
    }
    
    for(i=0; i<N; i++) {
        b[i] = a[i] + c[N-i-1];
    }
}
int main () {
    int a[N], b[N], c[N];

    for(int i = 0; i < N; i++)
        c[i] = 1;

    struct timespec start;

    clock_gettime(CLOCK_REALTIME, &start);

    for(int i = 0; i < N; i++)
        populate(a,b,c);
    
    struct timespec end;

    clock_gettime(CLOCK_REALTIME, &end);

    long double delta = (long double)(end.tv_sec - start.tv_sec) + (long double)(end.tv_nsec - start.tv_nsec)/(long double)(BILLION);

    printf("Time elapsed: %Lf\n\n", delta);
    return 0;
}