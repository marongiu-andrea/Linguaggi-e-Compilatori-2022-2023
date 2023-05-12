#include <stdio.h>

#define SIZE 10

void fun(int a[SIZE])
{
    for (int i = 0; i < SIZE; i++)
    {
        a[i] *= a[i];
    }

    printf("1° elemento dell'array: %d\n", a[0]);

    for (int i = 0; i < SIZE; i++)
    {
        printf("%d\n", a[i]);
    }

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 1; j < SIZE; j++)
        {
            if (a[i] > a[j])
            {
                int tmp = a[i];

                a[i] = a[j];
                a[j] = tmp;
            }
        }

        for (int i = 0; i < SIZE; i++)
        {
            printf("%d\n", a[i]);
        }
    }

    for (int i = 0; i < SIZE; i++)
    {
        printf("%d\n", a[i]);
    }
}