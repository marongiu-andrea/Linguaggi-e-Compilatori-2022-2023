#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#define N 100

// llc -filetype=obj -o Loop.opt.o Loop.opt.bc
// gcc main.c Loop.opt.o -o measurePerfTimeOpt

const int ARR_LEN = 100000;

extern void populate (int *, int*, int *);

int main()
{
    int a[ARR_LEN];
    int b[ARR_LEN];
    int c[ARR_LEN];

    struct timespec start;
    struct timespec stop;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < N; ++i)
    {
        populate(a, b, c);
    }

    clock_gettime(CLOCK_MONOTONIC, &stop);

    printf("Tempo: %i\n", (long long unsigned int)(stop.tv_nsec - start.tv_nsec));

    return 0;
}

