#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
void foo(FILE* fp) {
    int N = 4; // chunk size
    int M = 100; 	
	struct timespec start, finish;
	clock_gettime(CLOCK_REALTIME, &start);
    #pragma omp parallel for schedule(dynamic, N) num_threads(4)
    /* #pragma omp parallel for schedule(static) num_threads(4) */
	for (int i = 0; i < M; ++i) {	  
	  int randomnumber = (rand() % 10) + 1;
	  usleep(randomnumber * 10);
	}
	clock_gettime(CLOCK_REALTIME, &finish);
	double elapsed_time = ((double) (finish.tv_nsec - start.tv_nsec))/((double) 1000000);
	fprintf(fp, "%f\n",elapsed_time);
    printf("Exec time: %f \n", elapsed_time);
    return;
}
int main(){
  int num_ex = 15;
  FILE *fp = fopen("for_output_3.txt", "w");
  for(int i = 0; i < num_ex; i++){
	foo(fp);
  }
  return 0;
}
