# Comandi

## Compiliamo l'eseguibile

    make

## Ottimiziamo il programma

    opt -load-pass-plugin=./libLocalOpts.so -passes=transform testTransform/Foo.ll -o testTransform/Foo.optimized.bc

## Ottimiziamo il programma di test (Algebric)

    opt -load-pass-plugin=./libLocalOpts.so -passes=algebric test/test.ll -o test.optimized.bc
    
## Ottimiziamo il programma di test (StrengthReduction)

    opt -load-pass-plugin=./libLocalOpts.so -passes=strength_reduction test/test.ll -o test/test.optimized.bc

## Generiamo il file .ll per leggere il programma creato (test)

    llvm-dis test/test.optimized.bc -o test/test.optimized.ll

## Generiamo il file .ll per leggere il programma creato (Foo)

    llvm-dis testTransform/Foo.optimized.bc -o testTransform/Foo.optimized.ll