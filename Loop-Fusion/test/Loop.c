void populate(int a[], int b[], int c[], int size, int step)
{
    int i;
    for(i = 0; i < size; i += step)
        a[i] = 5*c[i];
    for(i = 0; i < size; i += step)
        b[i] = a[i]+c[i];
}