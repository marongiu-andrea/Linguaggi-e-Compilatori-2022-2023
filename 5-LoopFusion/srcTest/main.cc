#include "populate.h"
#include <iostream>
#include <time.h>
using namespace std;
int main(int argc, char *argv[])
{
    const char *mode = "base";
    if (argc == 2)
    {
        mode = argv[1];
    }
    int a[N], b[N], c[N];

    for (int i = 0; i < N; i++)
        c[i] = 1;

    // Misurazione del tempo di esecuzione
    clock_t start = clock();

    for (int i = 0; i < 20000; i++)
    {
        populate(a, b, c);
    }

    clock_t end = clock();
    double executionTime = double(end - start) / CLOCKS_PER_SEC;
    cout.precision(3);
    cout << "Tempo di esecuzione di populate() " << mode << " : " << executionTime << " secondi" << endl;

    return 0;
}
