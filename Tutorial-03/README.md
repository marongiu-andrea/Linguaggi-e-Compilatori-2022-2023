# Terzo Laboratorio: LoopWalk Pass

In questo laboratorio sviluppiamo il passo LoopWalkPass, che analizza un loop.
In particolare:
1) Analizza la struttura del loop e ne verifica la forma normale
2) scorre e stampa i basic blocks che lo compongono
3) scorre le istruzioni del loop e determina le UD/DU chains per determinate istruzioni

opt -enable-new-pm=0 -load ./libLoopWalk.so -loop-walk test/Loop.ll -o Loops.opt.bc
llvm-dis Loops.opt.bc -o Loops.opt.ll