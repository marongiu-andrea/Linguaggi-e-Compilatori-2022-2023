#include <stdio.h>
#include <time.h>

#define N 10000

// gcc main.c Loop.c -o measurePerfTime

// llc -filetype=obj -o Loop.opt.o Loop.opt.bc
// gcc main.c Loop.opt.o -o measurePerfTimeOpt

const int ARR_LEN = 100000;

extern void populate (int *, int*, int *);

int main()
{
    int a[ARR_LEN];
    int b[ARR_LEN];
    int c[ARR_LEN];

    struct timespec start, stop;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < N; ++i)
        populate(a, b, c);

    clock_gettime(CLOCK_MONOTONIC, &stop);

    long long elapsedTime = (stop.tv_sec - start.tv_sec) * 1e9 + (stop.tv_nsec - start.tv_nsec);
    printf("Tempo: %llu nanosecondi\n", elapsedTime);

    return 0;
}

