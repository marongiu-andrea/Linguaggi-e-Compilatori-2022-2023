#include <stdio.h>

void test(int* a, int* b, int* c, int n) {
    for (int i = 0; i < n; ++i)
        a[i] = 5 * c[i];

    for (int i = 0; i < n; ++i)
        b[i] = a[i] + c[i];
}