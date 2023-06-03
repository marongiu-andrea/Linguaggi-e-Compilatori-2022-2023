#include "populate.h"

void populate(int a[N], int b[N], int c[N])
{
    int i;
    for (i = 0; i < N; i++)
    {
        a[i] = 5 * c[N - i - 1];
        if (a[i] > 10)
        {
            b[i] = 5 * c[i];
        }
    }

    for (i = 0; i < N; i++)
    {
        b[i] = a[i] + c[N - i - 1];
    }
}
