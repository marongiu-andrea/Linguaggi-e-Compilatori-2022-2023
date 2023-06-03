#define SIZE 100000

void populate(int a[SIZE], int b[SIZE], int c[SIZE])
{
    for (int i = 0; i < SIZE; i++)
    {
        a[i] = i;
        b[i] = a[i] * 2;
    }

    for (int i = 0; i < SIZE; i++)
    {
        c[i] = b[i];
    }
}