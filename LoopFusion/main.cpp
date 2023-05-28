#include <time.h>
#include <cstdio>

extern void populate(int[], int[], int[]);

int main ()
{
	float x,y;
	clock_t time;
  const int dim=100;
  const int testSize=100000000;
  int a[dim];
  time = clock();
  for(int i=0;i<testSize;i++) populate(a,a,a);
	time = clock() - time;
  printf ("Using populate function, it took: %6.6f seconds \n", (float)time/CLOCKS_PER_SEC );

	return 0;
}


