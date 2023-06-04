#include <stdio.h>
#include <stdlib.h>

#include <time.h>

void test(int* a, int* b, int* c, int n);

void populate(int* r, int n) {
  for (int i = 0; i < n; ++i)
    r[i] = rand() % 100;
}

#define N 500

#define REPS 1000

long get_time() {
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
    return ts.tv_nsec;

  printf("Error. Exiting...");
  exit(1);
}


int main() {
  srand(0);

  int a[N];
  int b[N]; 
  int c[N];

  printf("Populating test data...\n");

  populate(a, N);
  populate(b, N);
  populate(c, N);

  printf("Beginning tests.\n");

  ulong time_acc;

  for (int i = 0; i < REPS; ++i) {
    
    long start = get_time();
    test(a, b, c, N);
    long end = get_time();

    time_acc += end - start;
  }

  ulong avg = time_acc / REPS;
  printf("On average, it took %d nanoseconds.\n", avg);
}