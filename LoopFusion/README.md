# Loop fusion

La **loop fusion** è un'ottimizzazione che ha lo scopo di migliorare l'utilizzo della cache da parte del programma.

Si prenda come esempio la seguente funzione `fun`:

``` c
#define SIZE 100

void fun(int a[SIZE], int b[SIZE], int c[SIZE])
{
    for (unsigned int i = 0; i < SIZE; i++)
    {
        a[i] = i;
        b[i] = a[i] * a[i];
    }

    for (unsigned int i = 0; i < SIZE; i++)
    {
        c[i] = b[i] + 1;
    }
}
```

I due loop iterano lo stesso numero di volte (`SIZE`), ovvero hanno lo stesso **trip count**.

In entrambi i loop si accede allo stesso valore `b[i]`, una volta in scrittura ed una volta in scrittura.

Scritti in questo modo, i loop non sfruttano per niente la cache. Il valore `b[i]` scritto nel 1° loop sarebbe già in cache per poter eseguire l'istruzione `c[i] = b[i] + 1` in maniera efficiente. Essendo però le due istruzioni in loop diversi, la cache non viene utilizzata.

Una versione più efficiente si avrebbe con la **fusione** dei due loop:
``` c
#define SIZE 100

void fun(int a[SIZE], int b[SIZE], int c[SIZE])
{
    for (unsigned int i = 0; i < SIZE; i++)
    {
        a[i] = i;
        b[i] = a[i] * a[i];
        c[i] = b[i] + 1; // b[i] è stato inserito in cache a seguito dell'istruzione precedente
    }
}
```

In generale, la fusione tra due loop $L_i, L_j$ può essere fatta solo se **tutte** le seguenti condizioni sono rispettate:
- i due loop sono **adiacenti**;
- i due loop devono iterare lo <u>stesso numero di volte</u>;
- i due loop devono essere **control flow equivalent** (quando $L_i$ esegue esegue anche $L_j$ e quando esegue $L_j$ esegue anche $L_i$);
- non possono esserci delle **negative distance dependencies** tra i due loop.
  - una negative distance dependency si ha quando un'iterazione $m$ di $L_j$ utilizza un valore calcolato in un'iterazione $m + n$ di $L_i$ (con $n > 0$ e $L_i$ predecessore di $L_j$)

## 1. Controllo sull'adiacenza

Due loop $L_i, L_j$ sono adiacenti se e solo se non ci sono altre istruzioni tra i due loop.

Il controllo sull'adiacenza l'abbiamo implementato in questo modo:

``` c++
bool LoopFusion::areLoopsAdjacent(const Loop* i, const Loop* j) const
{
    const BasicBlock* iExitBlock = i->getExitBlock();
    const BasicBlock* jPreheader = j->getLoopPreheader();

    return iExitBlock != nullptr &&
           jPreheader != nullptr &&
           iExitBlock == jPreheader &&
           jPreheader->getInstList().size() == 1;
    /*
     * Non c'è bisogno di controllare anche il tipo dell'istruzione contenuta nel preheader.
     * Per definizione, il preheader di un loop contiene un'istruzione branch all'header del loop.
     * Se quindi nel preheader c'è una sola istruzione, sicuramente quella è una branch.
     */
}
```

Per prima cosa, per semplificare il controllo, abbiamo considerato soltanto i loop che hanno una sola uscita. Per questi loop il metodo [`llvm::LoopBase::getExitBlock()`](https://llvm.org/doxygen/classllvm_1_1LoopBase.html#ab48af53a5000ecede46c76dabb4578d2) restituisce qualcosa `!= nullptr`.

Ci sono diversi casi in cui un loop ha più di un'uscita:
- presenza di `break` o `return` nel body del loop;
- invocazione di funzioni di libreria (es. `exit`), per le quali il compilatore non ha modo di sapere se alterano il control flow

Oltre ad essere più complesso, gestire i loop con più di un'uscita sarebbe stato anche abbastanza inutile. In tutti questi casi è molto probabile infatti che il compilatore non sia in grado di calcolare la trip count del loop (es. se la `break`/`return` dipende da un particolare valore presente nell'array su cui sta iterando), perciò la loop fusion non sarebbe comunque fattibile.

Abbiamo poi scelto di considerare soltanto i loop in forma **normalizzata** (quindi con un preheader).

Il controllo `jPreheader->getInstList().size() == 1` è necessario per verificare che non ci siano istruzioni tra i due loop inserite nel preheader di $L_j$.

## 2. Controllo sul numero di iterazioni

Il numero di iterazioni di un loop (**trip count**) si ottiene tramite la **scalar evolution analysis**. Come dice il nome, si tratta di un'analisi che cerca di capire l'evoluzione dei valori scalari (tra cui ad esempio la loop induction variable) durante il flusso d'esecuzione del programma.

Il metodo [`llvm::ScalarEvolution::getSmallConstantTripCount(llvm::Loop*)](https://llvm.org/doxygen/classllvm_1_1ScalarEvolution.html#abec0c616087c002528fcf80c6583eadd) permette di ottenere il trip count di un loop. Se questo non è calcolabile a compile time, viene restituito `0`.

Due loop iterano lo stesso numero di volte se:
- il trip count di entrambi è calcolabile (i.e. è `!= 0`);
- i due loop hanno lo stesso trip count

Per semplicità, la nostra implementazione si ferma a questi controlli.

In realtà però ci sarebbero da fare ragionamenti molto più complessi oltre al numero di iterazioni. Si prenda questo esempio:

``` c
#define SIZE 10

void fun(int a[SIZE], int b[SIZE])
{
    for (int i = 0; i < SIZE; i++)
    {
        a[i] = b[i];
    }

    for (int i = SIZE - 1; i >= 0; i--)
    {
        b[i] = i;
    }
}
```

I due loop iterano lo stesso numero di volte, ma lo **spazio delle iterazioni** è percorso in ordine inverso fra i due loop. La loop fusion in questo caso sarebbe più complessa, perché bisognerebbe anche modificare l'induction variable del 1° loop in modo da rispettare la semantica del programma.

## 3. Controllo sulla control flow equivalence

Due loop $L_i, L_j$ sono **control flow equivalent** se:
- quando esegue $L_i$ esegue anche $L_j$;
- quando esegue $L_j$ allora è stato eseguito anche $L_i$

In altre parole, affinché due loop siano control flow equivalent non devono esistere percorsi che permettano di eseguire uno solo fra $L_i$ ed $L_j$.

Questo controllo si effettua tramite l'analisi sul **dominator tree** e sul **post-dominator tree**.

<!-- TODO: mettere il codice -->

## 4. Controllo sulle dipendenze di dato

Poiché il controllo sulle dipendenze è molto complesso, nella nostra implementazione abbiamo dato per scontato che questa condizione sia sempre verificata:

<!-- TOOD: inserire code snippet -->


# Generazione di un file di test

1. scrivere il file `.c`;
2. compilarlo con `clang -O0 -Xclang -disable-O0-optnone -emit-llvm -S -c <source file> -o <output file>`;
3. eseguire `opt -passes=mem2reg -S <source file> -o <output file>`

