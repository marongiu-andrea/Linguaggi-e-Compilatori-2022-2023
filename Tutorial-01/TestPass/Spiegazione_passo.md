Abbiamo esteso il passo TestPass in modo che analizzi la IR e stampi alcune informazioni tra cui:

1. Nome
2. Numero di argomenti (‘N+’ in caso di funzione variadica)()
3. Numero di chiamate a funzione nello stesso modulo
4. Numero di Basic Blocks
5. Numero di Istruzioni

Per far ciò, abbiamo creato un for, dove andavamo a prendere, una alla volta, le istruzioni all'interno di un modulo.
Da questa istruzione ne ricaviamo il nome della funzione e i suoi parametri, attraverso chiamate a funzioni.
Dopodichè inizializziamo delle variabili per tenere conto di quante chiamate vengono fatte all'interno delle funzioni, e si fa partire un altro for, dove verranno incrementati i valori a seconda della situazione.
E infine andiamo a stampare i valori ottenuti dal for precedente.
