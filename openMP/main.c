#include <stdio.h>
#include <omp.h>

void ex2()
{
	int id;
	int base = 5;
	int slaves = 0;
	
	#pragma omp parallel firstprivate (base, slaves) private (id)
	{
		id = omp_get_thread_num();
		slaves = omp_get_num_threads() - 1;

		if (id == 0)
			printf("Master: base = %d\n", base*slaves);
		else
			printf("Slave: base = %d\n", base*id);
	}
}

int main()
{
	ex2();
	return 0;
}
