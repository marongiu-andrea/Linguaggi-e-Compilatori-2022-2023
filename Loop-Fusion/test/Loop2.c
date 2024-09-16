void populate(int a[], int b[], int c[], int size, int step)
{
    int i;
    int j;
    int max = size * step;
    for(i = 0; i < max; i += step) {
        j = (i % size);
        a[j] = 5*c[j];
    }

    for(i = 0; i < max; i += step) {
        j = (i % size);
        b[j] = a[j]+c[j];
    }
}