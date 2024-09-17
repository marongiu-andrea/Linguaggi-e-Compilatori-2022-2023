#include "test.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITERATIONS_COUNT 1000

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

    for (size_t i = 0; i < ITERATIONS_COUNT; i++)
    {
        populate(a, b, c);
    }

    return 0;
}
