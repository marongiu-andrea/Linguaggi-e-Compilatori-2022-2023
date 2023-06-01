#include <stdio.h>
#include <time.h>

void main() {
    int N = 10;
    int M = 12;
    int NITERS = 50;
    int a = 0;

    clock_t start = clock();

    #pragma omp for schedule(dynamic, N)
    {

        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < NITERS; ++j) {
                ++a;
            }
        }

    }

    clock_t stop = clock();

    double time = (stop - start) / (double) CLOCKS_PER_SEC;
    printf("Exec time: %f \n", time);

    return;
}