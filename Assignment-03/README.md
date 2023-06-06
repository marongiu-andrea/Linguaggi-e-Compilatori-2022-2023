# Terzo Assignment: Loop Invariant Code Motion

In questo laboratorio sviluppiamo il passo LoopWalkPass, che analizza un loop.
In particolare:
1) Analizza la struttura del loop e ne verifica la forma normale
2) scorre e stampa i basic blocks che lo compongono
3) scorre le istruzioni del loop e determina le UD/DU chains per determinate istruzioni


# Comandi per riuscire a compilare ed eseguire correttamente il programma

Lanciare i seguenti comandi in successione, per riuscire a produrre in output un valido file che contiene la nuova versione del codice, dopo lo spostamento delle istruzini Loop Invariant e Hoistable:

1) make
2) opt -enable-new-pm=0 -load ./libLoopWalk.so -loop-walk test/Loop.ll -o lib/result.bc
3) llvm-dis test/result.bc -o test/result-optimized.ll