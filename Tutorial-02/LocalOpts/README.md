# Osservazioni 

L'operazione di ``` mult x y ```
viene sostituita con 
``` shift x log2(y) ```
se y è potenza di 2.

Per poter eliminare totalmente l'istruzione mult è necessario prima inserire la nuova istruzione shl e sostituire tutti gli usi di mult con il risultato della nuova istruzione.

Nota: per rimuovere la mult è stato necessario un vettore d'appoggio perchè no si può eliminare un istruzione da un basic block che si sta iterando con un ``` for( : ) ```