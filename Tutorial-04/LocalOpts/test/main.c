#include "Loop.cpp"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITERATIONS_COUNT 1000
#define ARRAY_SIZE 100

extern void populate(int* a, int* b, int* c);

int main(int argc, char** argv)
{
    int* a = (int*)calloc(ARRAY_SIZE, sizeof(int));
    int* b = (int*)calloc(ARRAY_SIZE, sizeof(int));
    int* c = (int*)calloc(ARRAY_SIZE, sizeof(int));

    if (a == NULL || b == NULL || c == NULL)
    {
        fprintf(stderr, "Si è verificato un errore durante l'allocazione degli array. Impossibile proseguire.");

        return 1;
    }

    clock_t start_time = clock(); // Ottieni il tempo di inizio

    for (size_t i = 0; i < ITERATIONS_COUNT; i++)
    {
        populate(a, b, c);
    }

    clock_t end_time = clock(); // Ottieni il tempo di fine

    double total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC; // Calcola il tempo totale in secondi

    printf("%f", total_time);

    // Deallocazione della memoria
    free(a);
    free(b);
    free(c);

    return 0;
}
