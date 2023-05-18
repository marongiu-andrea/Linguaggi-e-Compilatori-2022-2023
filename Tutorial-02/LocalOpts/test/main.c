#include "time.h"
#include "stdio.h"

#define N 100
extern void populate(int*, int*, int*);

void main(){
    
    int a[N];
    int b[N];
    int c[N];
    clock_t start = clock();
    for(int i=0; i<N; i++){
        populate(a,b,c);
    }
    clock_t end = clock();

    double enlapse= (double)(end- start) / CLOCKS_PER_SEC;
    
    printf("Time : %F nanosec", enlapse);
    
}