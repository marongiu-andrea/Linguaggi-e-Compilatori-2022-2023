#define N 100
#include <stdlib.h>
#include <stdio.h>

void populate(int a[N], int b[N], int c[N])
{
    int i;
    for (i = 0; i < N; i++)
        a[i] = 5 * c[i];

    for (i = 0; i < N; i++)
        b[i] = a[i] + c[i];
}