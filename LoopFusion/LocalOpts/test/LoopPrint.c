#include <stdio.h>
#define N 100

void populate (int a[N], int b[N], int c[N])
{
	int i;

	for (i=0; i<N; i++)
		a[i] = 5*c[i];

	printf("a[1]: %i", a[1]);

	for (i=0; i<N; i++)
		b[i] = a[i]+c[i];		
}