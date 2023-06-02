#include <stdio.h>
#include <time.h>

void main() {
    int N = 1;
    int M = 12;
    int NITERS = 10;

    clock_t start = clock();

    #pragma omp parallel for schedule(dynamic, N) num_threads(4)

    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < NITERS; ++j) {
            printf("%d\n", j);
        }
    }

    clock_t stop = clock();

    double time = (stop - start) / (double) CLOCKS_PER_SEC;
    printf("Exec time: %f \n", time);

    return;
}