#include "time.h"
#include <time.h>
#include <stdio.h>
#define N 100000

extern void populate(int [], int [], int [] );

int main(){

    int a[N];
    int b[N];
    int c[N];

    clock_t initial_t = clock();
    populate(a, b, c);
    clock_t final_t = clock();

    double time_diff = ((double)(final_t-initial_t))/CLOCKS_PER_SEC;

    printf("Time delta: %F\n", time_diff);
}
