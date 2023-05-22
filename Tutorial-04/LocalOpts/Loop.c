
void populate (int a[], int b[], int c[], int N, int S) {
    int i;

    for (i=0; i<N; i+=S)
        a[i] = 5*c[i];
    
    for (i=0; i<N; i+=S)
        b[i] = a[i]+c[i];
}