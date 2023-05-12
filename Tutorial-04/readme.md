# Comandi

## Compiliamo l'eseguibile

    make

## Ottimiziamo il programma

    opt -enable-new-pm=0 -load ./libLoopInvariantCodeMotion.so -loop-invariant-code-motion test/Loop.ll -disable-output

## Ottimizza il progrmma con output
    
    opt -enable-new-pm=0 -load ./libLoopInvariantCodeMotion.so -loop-invariant-code-motion test/Loop.ll -o test/Loop.optimized.bc

   
## Generiamo il file .ll per leggere il programma creato
    
    llvm-dis test/Loop.optimized.bc -o test/Loop.optimized.ll