# Comandi

## Generiamo il file
## Primo comando

    clang-14 -O0 -Xclang -disable-O0-optnone -emit-llvm -c test.cpp -o test/Loop.bc


## Ottimiziamo il programma

    opt -passes=mem2reg test/loop.bc -o test/test.base.bc


## Generiamo il file .ll per leggere il programma creato (Foo)

    llvm-dis test/Loop.bc -o test/Loop.ll

## Compiliamo l'eseguibile

    make

## Ottimiziamo il programma

    opt -enable-new-pm=0 -load ./libLoopFusion.so -loop-fusion-pass test/Loop.ll -disable-output

## Ottimizza il progrmma con output

    opt -enable-new-pm=0 -load ./libLoopFusion.so -loop-fusion-pass test/Loop.ll -o test/Loop.fused.bc

   
## Generiamo il file .ll per leggere il programma creato
    
    llvm-dis test/Loop.fused.bc -o test/Loop.fused.ll