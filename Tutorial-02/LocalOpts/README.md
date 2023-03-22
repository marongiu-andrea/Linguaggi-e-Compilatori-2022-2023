# Comandi

## Compiliamo l'eseguibile

    make

## Ottimiziamo il programma

    opt -load-pass-plugin=./libLocalOpts.so -passes=transform testTransform/Foo.ll -o testTransform/Foo.optimized.bc

## Generiamo il file .ll per leggere il programma creato

    llvm-dis testTransform/Foo.optimized.bc -o testTransform/Foo.optimized.ll