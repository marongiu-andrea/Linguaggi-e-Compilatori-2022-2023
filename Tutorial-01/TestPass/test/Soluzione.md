# Gruppo del fork SimoneIOLST

Matricole 152532, 152240, 152569

loop.c, loop.ll
--------------------------------------

Il loop con l’opzione -O2 è stato eliminato  in quanto viene eseguito di sicuro b-a volte. Inoltre, la chiamata a funzione g_incr è “inlined” diventando g+=c. Infine toglie il ret + g restituendo direttamente g stesso. Proponiamo un'idea di codice C equivalente al risultato dell'ottimizzazione di LLVM. Qua sotto uno pseudocodice e non esattamente come viene generato dalla LLVM.

Il CFG del file è messo come "loop_solution.pdf" nella cartella.

``` c
int loop(int a, int b, int c)
{
    // int i, ret = 0;

    // for (i = a; i < b; i++) {
    //   g_incr(c);
    // }

    if (b > a)
        g += (b - a) * c;

    // return ret + g;
    return g;
}
```

Con l'opzione -O0 invece genera il codice intermedio che rispetta esattamente le istruzioni scritte nel sorgente poiché non esegue ottimizzazione di alcun tipo.


Fibonacci.c, Fibonacci.ll
-----------------------------------------
Le ottimizzazioni effettuate trasformano parte della ricorsione in loop, permettendo di fare una sola chiamata ricorsiva per iterazione. Viene fatto un uso intelligente delle istruzioni phi per risparmiare variabili d'appoggio e/o istruzioni di branch condizionato.

E' complesso produrre un codice C equivalente, a meno di modifiche alla signature della funzione Fibonacci. Anche in tal caso non si arriverebbe ad avere una soluzione altrettanto valida.

Il CFG del file è in "fibonacci_solution.pdf" nella cartella.

N.B. La chiamata alle funzioni di sistema non permettono al compilatore di effettuare l'inlining ed altre ottimizzazioni, perchè non recupera il codice sorgente delle stesse
