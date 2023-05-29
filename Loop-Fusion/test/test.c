#include <stdio.h>

#define n 100
void test(int a[n], int b[n], int c[n]) {
    int i;

    for(i=0; i<n; i++) {
        a[i] = 5*c[i];
    }

    for(i=0; i<n; i++) {
        b[i] = a[i]+c[i];
    }
}