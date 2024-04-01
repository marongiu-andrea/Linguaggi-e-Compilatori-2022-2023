# Tutorial 4

<aside>
📝 Gruppo: Gabriele Cusato, matricola 152669, e Alessandro Giacconi, matricola 152533.
</aside>

---
## Loop Fusion
Il programma che noi abbiamo scritto si occupa di effettuare la Loop Fusion, affinchè tale operazione venga effettuata devono essere rispettati le seguenti condizioni:

- I due loop devono essere adiacenti, non devono esserci istruzioni tra i loop.
- Devono iterare lo stesso numero di volte.
- Devono essere control flow equivalent.

> In verità bisognerebbe anche controllare che non vi siano dipendenze di dato, ma per motivi di praticità questa condizione si considera vera.

## I loop da fondere
Nel file **Loop.cpp** è presente il codice dei due loop che verranno analizzati e, se rispetteranno le condizioni citate qua sopra, verranno fusi insieme.

```c++
#define N 100

void populate (int a[N], int b[N], int c[N])
{
    int i;
    for(i=0; i<N; i++)
        a[i] = 5*c[i];

    for(i=0; i<N; i++)
        b[i] = a[i]+c[i];
}
```
I due loop itereranno un numero di volte pari a 100, quindi possiedono lo stesso trip count.

Nel modo in cui i loop si presentano non riescono a sfruttare la cache, perchè anche se il dato sarebee già in cache essendo in loop diversi quest'ultima non viene utilizzata.

## Controllo condizioni
### 1 - Controllo adiacenza

Questo controllo è stato implementato attraverso una apposita funzione **TransformPass::isPreHeaderExitBlock(BasicBlock\* block1, BasicBlock\* block2)**.

Essa viene chiamata nella funzione run nel seguente modo:

```llvm
isPreHeaderExitBlock(L1->getExitBlock(), L2->getLoopPreheader())
```

che è stata messa in AND con le altre condizioni da rispettare.

La funzione si presenta nel seguente modo:

```llvm
bool TransformPass::isPreHeaderExitBlock(BasicBlock* block1, BasicBlock* block2) {
  if(block1 != nullptr && block2 != nullptr && block1 == block2 && (block2->size() == 1)) 
  {
    assert((block2->begin()->getOpcode()) == Instruction::Br);
    return true;
  }
  return false;
}
```

Al fine di non complicare i controlli si sono solo considerati i loop aventi un solo blocco di uscita, per questo viene fatto il controllo per il **block1** e **block2** che siano diversi da **nullptr**.

Inoltre dopo aver controllato che i due blocchi siano lo stesso, viene fatto un'ulteriore verifica riguardo al **block2**, viene controllato se è presente l'istruzione di salto essendo una blocco preheader.

### 2 - Controllo Trip Count

Per effettuare la verifica del numero di iterazioni dei singoli loop abbiamo utilizzato la **scalar evolution analysis**. Scalar Evolution o SCEV come viene spesso abbreviato, è, in un senso molto ampio, un'analisi del cambiamento (da qui la parte "evoluzione") delle quantità scalari in un programma.

L'oggetto SCEV viene creato nel modo seguente:

```llvm
ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
```

Come per il controllo dell'adiacenza è stata creata una funzione apposita per gestire questo accertamento e si chiama **TransformPass::equalTripCount(Loop \*L1, Loop \*L2, ScalarEvolution &SE)**.

La funzione viene chiamata in AND con le altre condizioni nel seguente modo:
```llvm
equalTripCount(L1, L2, SE)
```

La funzione si presenta nel seguente modo:

```llvm
bool TransformPass::equalTripCount(Loop *L1, Loop *L2, ScalarEvolution &SE)
{
    if(SE.getSmallConstantTripCount(L1) != 0 && SE.getSmallConstantTripCount(L2) != 0 && SE.getSmallConstantTripCount(L1) == SE.getSmallConstantTripCount(L2))
    return true;
  return false;
}
```

Il metodo **getSmallConstantTripCount** permette di ricavare il trip count del loop passato come parametro e nel caso in cui non fosse calcolabile restituirebbe 0. Quindi viene effettuato il controllo per vedere che i due loop abbiano trip count calcolabile, ovvero **!= 0**.

### Control flow equivalence

I due loop vengono considerati **control flow equivalent** se:

- Il loop **L1** domina il loop **L2**
- Il loop **L2** post-domina il loop **L1**

Ovvero se esegue uno dei due loop, anche l'altro dovrà essere eseguito.

La funzione che gestisce questo aspetto si chiama **TransformPass::controlFlowEq(BasicBlock\* block1, BasicBlock\* block2,  DominatorTree &DT, PostDominatorTree &PDT)**.

Gli oggetti che vengono utilizzati per controllare che la condizione venga rispettata sono il **dominator tree** e il **post-dominator tree** e vengono generati nel seguente modo:

```llvm
DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F); 
```

La funzione viene chiamata in AND con le altre condizioni nel seguente modo:
```llvm
controlFlowEq(L1->getLoopPreheader(),L2->getLoopPreheader(),DT,PDT)
```

La funzione si presenta nel seguente modo:

```llvm
bool TransformPass::controlFlowEq(BasicBlock* block1, BasicBlock* block2,  DominatorTree &DT, PostDominatorTree &PDT )
{
  if(DT.dominates(block1, block2) && PDT.dominates(block2, block1)) //
    return true;
  return false;
}
```

## Fase di merge
### Trasformazione delle induction variables

le induction variables del secondo loop vengono cambiate con quelle del primo loop: 
```llvm
PHINode *InductionVar1 = L1->getCanonicalInductionVariable();
PHINode *InductionVar2 = L2->getCanonicalInductionVariable();

//sostituisco la seconda induction variable con la prima
InductionVar2->replaceAllUsesWith(InductionVar1);
```

poi si va a sostituire l'uscita del primo loop con l'uscita del secondo loop (in modo da inglobare il secondo loop):
```llvm
L1->getHeader()->getTerminator()->replaceSuccessorWith(L1->getExitBlock(),L2->getExitBlock());
```

dopodichè recupero il blocco di entrata del secondo loop per poterlo collegare successivamente al body del primo loop: 
```llvm
BasicBlock* L2BodyEntry = nullptr;
BasicBlock* L2Header = L2->getHeader();

for (BasicBlock* succ : successors(L2Header))
{
    if (L2->contains(succ))
    {
        L2BodyEntry = succ;
        break;
    }
}
```

Si effettua quindi il collegamento fra il body del primo loop e il body del secondo loop: 
```llvm
BasicBlock* primaryLatch = L1->getLoopLatch();

//prendo il predecessore del latch del primo loop ovvero l'ultimo blocco del body del primo loop
for (BasicBlock* pred : predecessors(primaryLatch))
{
    pred->getTerminator()->replaceSuccessorWith(primaryLatch, L2BodyEntry);
}
```

poi si va a sostituire il collegamento fra l'ultimo blocco del body e il secondo latch con il latch del primo loop:
```llvm
BasicBlock* secondaryLatch = L2->getLoopLatch();

for (BasicBlock* pred : predecessors(secondaryLatch))
{
    pred->getTerminator()->replaceSuccessorWith(secondaryLatch, primaryLatch);
}
```

infine vado a collegare l'header del secondo loop al latch del secondo loop in modo da non dover cancellare i blocchi inutilizzati, perchè li cancellerà automaticamente llvm in delle fasi di ottimizzazione successive: 
```llvm
L2->getHeader()->getTerminator()->replaceSuccessorWith(L2BodyEntry, secondaryLatch);
```

poi si aggiustano i collegamenti del body del secondo loop, collegandoli al primo loop:
```llvm
for (auto& bb : L2->blocks())
{
    // header e latch non devono essere spostati
    if (bb != L2Header && bb != secondaryLatch)
    {
        L1->addBasicBlockToLoop(bb, LI);
        L2->removeBlockFromLoop(bb);
    }
}
```

alla fine si mettono a posto anche i loop innestati se ce ne sono e si cancella il secondo loop: 
```llvm
while (!L2->isInnermost())
{
    // prendo solo il primo figlio e continuo fin quando non ho finito
    Loop* child = *(L2->begin());
    L2->removeChildLoop(child);
    L1->addChildLoop(child);
}

// Rimuovere il loop 2
LI.erase(L2);
```

## Fase finale

dopo aver fatto la fase di merge si va a ripetere l'analisi di llvm per fare altri merge in maniera iterativa se necessario (change=true serve per ripetere il ciclo nel codice)
```llvm
AM.invalidate(F, PreservedAnalyses::none());
change = true;
```

## Test tempo di esecuzione

abbiamo creato un file ```main.cc``` accompagnato da un piccolo script ```compare_time.sh``` nella catella test, per visualizzare i tempi di esecuzione dei programmi ottimizzati e non ottimizzati, insieme alla loro differenza di tempo: 
```bash
#comando per eseguire lo script
./compare_time.sh

#output
Il programma NON ottimizzato ha impiegato  0.000721  secondi
Il programma CON ottimizzazione ha impiegato  0.000598  secondi

Differenza di tempo di esecuzione tra i due programmi: .000123 secondi
```
il comando per eseguire generare il codice ottimizzato partendo dalla cartella ```LocalOpts``` è il seguente: 
```bash
opt -load-pass-plugin=./libLocalOpts.so -passes=transform test/Loop.opt.ll -S -o test/test.optimized.ll
```