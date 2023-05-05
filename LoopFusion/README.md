# Comandi



## Primo comando
    clang-14 -O0 -Xclang -disable-O0-optnone -emit-llvm -c test.cpp -o test/test.bc


## Ottimiziamo il programma

    opt -passes=mem2reg test/test.bc -o test/test.base.bc


## Generiamo il file .ll per leggere il programma creato (Foo)

    llvm-dis test/test.base.bc -o test/test.ll

## Compiliamo l'eseguibile

    make

## Ottimiziamo il programma

    opt -load-pass-plugin=./libLocalOpts.so -passes=transform test/test.ll -o test/Test.optimized.bc

## Generiamo il file .ll per leggere il programma creato (Foo)

    llvm-dis test/Test.optimized.bc -o test/Test.optimized.ll