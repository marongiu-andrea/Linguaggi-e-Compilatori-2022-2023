#include <stdio.h>

#define N 50

void test(int* a, int* b, int* c) {
    for (int i = 0; i < N; ++i)
        a[i] = 5 * c[i];

    for (int i = 0; i < N; ++i)
        b[i] = a[i] + c[i];
}