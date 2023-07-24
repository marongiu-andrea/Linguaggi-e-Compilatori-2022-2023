/*
COMANDI PER CREARE IL .ll
clang -O0 -Xclang -disable-O0-optnone -emit-llvm -c Loop.cpp
opt -passes=mem2reg Loop.bc -o Loop.opt.bc
llvm-dis Loop.opt.bc -o Loop.opt.ll

mem2reg cerca di trasformare in registri ssa le variabili stack al posto di avere load store
questo è quello più importante quindi applico solo questo
*/

/*
PER PROVARE IL PASSO
opt -load-pass-plugin=./libLocalOpts.so -passes=transform test/Loop.opt.ll -o test/test.optimized.ll
*/
#define N 100

void populate (int a[N], int b[N], int c[N])
{
    int i;
    for(i=0; i<N; i++)
        a[i] = 5*c[i];

    for(i=0; i<N; i++)
        b[i] = a[i]+c[i];
}