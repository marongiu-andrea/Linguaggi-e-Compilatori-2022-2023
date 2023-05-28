#include "time.h"
#include "stdio.h"

#define N 1000000000
void populate(int *, int *, int *, int);

int main()
{
	int a[N];
	int b[N];
	int c[N];

	for(int i = 0; i < N; i++)
	{
		c[i] = i;
	}

	clock_t start = clock();
	populate(a,b,c, N);
	clock_t end = clock();

	double elapsed = (double)(end- start) / CLOCKS_PER_SEC;

	printf("Time : %F nanosec\n", elapsed);

	return 0;
}
