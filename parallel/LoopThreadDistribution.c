#include <stdio.h>
#include <omp.h>
#include <time.h>

// gcc -O0 LoopThreadDistribution.c -o noParallelDistribution 
// gcc -O0 -fopenmp -fdump-tree-ompexp  LoopThreadDistribution.c -o staticParallelDistribution

// gcc -O0 -fopenmp -fdump-tree-ompexp  LoopThreadDistribution.c -o dynamicParallelDistribution
void task()
{
    struct timespec start, stop;
    clock_gettime(CLOCK_MONOTONIC, &start);
    int numThread = 6;
	int N = 1000;
    int NITERS = 1000000; // relativo alla grandezza dei chunk

    int test = 0;
    // N nello statico = lo uso per dimensionarlo come una riga di cache per evitare di fare delle miss 
    // N nel dynamic = ogni volta che assegno del lavoro a un thread gli dò N chunk
	#pragma omp parallel num_threads(numThread)
    #pragma omp for schedule(static) 
        for (int j = 0; j < N; j++)
        {
            for (int i = 0; i < NITERS; i++) {test = 1;}
        }

    clock_gettime(CLOCK_MONOTONIC, &stop);

    long long elapsedTime = (stop.tv_sec - start.tv_sec) * 1e9 + (stop.tv_nsec - start.tv_nsec);
    printf("Tempo: %llu nanosecondi\n", elapsedTime);
}

int main()
{
	task();
	return 0;
}
