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
    int numThread = 8;
	int N = 1000;
    int N_ITERS = 1000000;

    int result[N_ITERS];

    // static: Iterations of a loop are divided into chunks of size ceiling(number_of_iterations/number_of_threads). Each thread is assigned a separate chunk.
    // static,n: Iterations of a loop are divided into chunks of size n. Each chunk is assigned to a thread in round-robin fashion.
    // dynamic: Iterations of a loop are divided into chunks of size ceiling(number_of_iterations/number_of_threads).
    // dynamic,n: As above, except chunks are set to size n: assegno a ogni thread n iterazioni alla volta

    // #pragma omp parallel for schedule(dynamic, 1) is equivalent to #pragma omp parallel for schedule(dynamic)
    // N: dimensione dei chunk

	#pragma omp parallel num_threads(numThread)
    #pragma omp for schedule(dynamic, 1) 
    for (int j = 0; j < N; j++)
    {
        for (int i = 0; i < N_ITERS; i++) 
        {
            result[i] = j;
        }
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
