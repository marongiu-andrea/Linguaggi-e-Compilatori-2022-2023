#!/bin/bash

#clang main.c Loop.opt.ll -o notOptTime.out
#clang main.c test.optimized.ll -o OptTime.out

# Esegue il primo programma e memorizza il tempo di esecuzione
output1=$(./notOptTime.out)
echo -e "\nIl programma NON ottimizzato ha impiegato " $output1 " secondi"
time_diff1=$(echo "$output1" | grep -oE '[0-9]+\.[0-9]+')

# Esegue il secondo programma e memorizza il tempo di esecuzione
output2=$(./OptTime.out)
echo -e "Il programma CON ottimizzazione ha impiegato " $output2 " secondi\n"
time_diff2=$(echo "$output2" | grep -oE '[0-9]+\.[0-9]+')

# Calcola la differenza tra i tempi di esecuzione dei due programmi
time_diff=$(echo "$time_diff1 - $time_diff2" | bc)

# Stampa il risultato
echo -e "Differenza di tempo di esecuzione tra i due programmi: $time_diff secondi"