# Lab 1 - Introduzione ai passi LLVM 

Il repository contiene la cartella "lib" e la cartella "test".
Nella prima si trova lo scheletro del passo TestPass (TestPass.cpp), che dovrà essere esteso come da consegna (vedi punti TODO nel codice)
Nella seconda si trovano due programmi di test (Loop.c, Fibonacci.c), che utilizzeremo per validare il passo.

# Istruzioni per la compilazione

Prerequisiti:

| Pacchetto | Versione richiesta         |
| --------- | -------------------------- |
| `make`    | la più recente disponibile |
| `llvm`    | 14 o 15                    |
| `clang`   | 14 o 15                    |

1. installare i pacchetti indicati nei prerequisiti (seguire la documentazione relativa alla propria distro);
2. clonare il repository;
3. eseguire il comando `make all` per generare le IR (sia ottimizzata che non ottimizzata) dei due file `test/Loop.c`, `test/Fibonacci.c` e la libreria `libTestPass.so` per `opt`

# Test del plugin

1. generare l'IR con `make`;
2. eseguire il comando `opt -load-pass-plugin=./libTestPass.so -passes=test-pass $IR_FILE -disable-output` (dove `$IR_FILE` è uno dei file che vengono generati da `make` nella directory `ir`)