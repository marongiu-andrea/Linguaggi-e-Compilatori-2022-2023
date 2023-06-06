#include "test.h"

void populate(int a[ARRAY_SIZE], int b[ARRAY_SIZE], int c[ARRAY_SIZE])
{
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        a[i] = i;
        b[i] = a[i] * 2;
    }

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        c[i] = b[i];
    }
}