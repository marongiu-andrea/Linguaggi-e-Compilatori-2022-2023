make
#OTTIMIZZAZIONE SENZA OUTPUT
#opt -enable-new-pm=0 -load ./libLoopInvariantCodeMotion.so -loop-invariant-code-motion test/Loop.ll -disable-output
#OTTIMIZZAZIONE CON OUTPUT
opt -enable-new-pm=0 -load ./libLoopInvariantCodeMotion.so -loop-invariant-code-motion test/Loop.ll -o test/Loop.optimized.bc