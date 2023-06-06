#include <time.h>
#include <stdio.h>

#define M 100000000

extern void populate(int[], int[], int[]);

int main() {

    int i;
    int a[100];
    int b[100];
    int c[100];

    clock_t start = clock();

    for(i=0;i<M;i++){
        populate(a,b,c);
    }

    clock_t end = clock();
    double elapsed_time = (end-start)/(double)CLOCKS_PER_SEC;
    printf("%f\n",elapsed_time);

    return 0;
}