#include <stdio.h>
#include <time.h>


extern void populate(int *, int *, int *);

int main(){
    static int N = 10000;

    int a[N];
    int b[N];
    int c[N];

    for(int i=0; i<N; i++){
        c[i] = i;
    }

    clock_t start = clock();
    populate(a,b,c);
    clock_t end = clock();

    double elapesd_time= ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Elapsed Time: %F\n", elapesd_time);

    return 0;
}