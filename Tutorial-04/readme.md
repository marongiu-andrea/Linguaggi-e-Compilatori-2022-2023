# Comandi

## Compiliamo l'eseguibile

    make

## Ottimiziamo il programma

    opt -enable-new-pm=0 -load ./libLoopInvariantCodeMotion.so -loop-invariant-code-motion test/Loop.ll -disable-output

    
