#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern void populate(int* a, int* b, int* c);
#define N 1000

int main(int argc, char** argv)
{
    int* a = (int*)calloc(sizeof(int), SIZE);
    int* b = (int*)calloc(sizeof(int), SIZE);
    int* c = (int*)calloc(sizeof(int), SIZE);

    clock_t start, end;
    double cpu_time_used;
    start = clock();
    for (int i = 0; i < N; i++)
        populate(a, b, c);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("for loop took %f seconds to execute \n", cpu_time_used);

    return 0;
}
