#include <stdio.h>
#include <time.h>
<<<<<<< HEAD
#include <unistd.h>
#include <stdlib.h>
void foo(FILE* fp) {
    int N = 5; // chunk size
    int M = 500; // static number of iterations
	struct timespec start, finish;
	clock_gettime(CLOCK_REALTIME, &start);    
    #pragma omp parallel for schedule(dynamic, N) num_threads(4)
	/* #pragma omp parallel for schedule(static) */
	for (int i = 0; i < M; ++i) {
	  /* printf("performing iteration: %d\n", i); */
	  int randomnumber = (rand() % 6) + 1;
	  usleep(randomnumber * 100);
	}
	clock_gettime(CLOCK_REALTIME, &finish);
	double elapsed_time = ((double) (finish.tv_nsec - start.tv_nsec))/((double) 1000000);
	/* double elapsed_time = ((double) (finish.tv_nsec - start.tv_nsec));     */
	fprintf(fp, "%f\n",elapsed_time);
    printf("Exec time: %f \n", elapsed_time);
    return;
}
int main(){
  int num_ex = 10;
  FILE *fp = fopen("for_output_2.txt", "w");
  for(int i = 0; i < num_ex; i++){
	foo(fp);
  }
  return 0;
}
