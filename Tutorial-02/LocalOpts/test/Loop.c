#define N  1000000000

void populate (int a[N], int b[N], int c[N]){
    int i;
    for (i=0; i<N; i++){
        a[i] = 5*c[i];
    }
    //printf("%d",a[i]);

    for (i=0; i<N; i++){
        b[i] = a[i]+c[i];
    }
}