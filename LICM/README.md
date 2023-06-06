# Terzo Assignment: Loop Invariant Code Motion
L'algoritmo di ottimizzazione Loop Invariant Code Motion si divide in 4 passi:
- recupero informazioni di analisi
- individuazione delle istruzioni loop invariant
- individuazione istruzioni hoistable
- esecuzione code motion

Ogni passo verrà discusso di seguito.

## Recupero informazioni di analisi
Il passo richiede le informazioni di 2 passi di analisi:
- passo LoopInfo, per determinare i loop;
- passo Dominator Tree Analysis, per determinare l'albero di dominanza.

La code motion può essere applicata solo ai loop che sono in _forma semplice_, ossia hanno un preheader, un singolo backedge e un singolo blocco di latch, uno o più exiting block.
Se così non fosse, il passo non viene applicato.

## Individuazione istruzioni loop invariant
Una istruzione del tipo **A = B + C** è loop invariant se:
- Le reaching definitions dei suoi operandi si trovano fuori dal loop;
- ogni operando ha esattmente una reaching definition all'interno del loop che è già stata marcata loop invariant.

Possiamo iniziare esaminando le istruzioni del loop e i suoi blocchi: vogliamo determinare quali blocchi sono "exiting", ossia hanno successori fuori dal loop,
e la natura degli operandi delle istruzioni.
```cpp
for (auto* bb : this->currentLoop->getBlocks())
{
    std::set<Instruction*> bbLoopInvariantInstructions = getLoopInvariantInstructions(bb);
    this->loopInvariantInstructions.insert(bbLoopInvariantInstructions.begin(), bbLoopInvariantInstructions.end());
    if (isLoopExiting(bb))
    {
        this->loopExitingBlocks.insert(bb);
    }
}
```
Controlliamo le condizioni degli operandi per ogni istruzione del loop.
Per ottenere le definizioni degli operandi, ossia la use-def chain, si utilizza `operands()`.
```cpp
bool LoopInvariantCodeMotionPass::isLoopInvariant(const Instruction& instr)
{
    return all_of(instr.operands(), [this](auto& operand) {
        return isLoopInvariant(operand);
    });
}
```
Andiamo a gestire direttamente i casi a cui siamo interessati: consideriamo operandi che siano costanti (`Constant`), argomenti alla funzione corrente (`Argument`)o altre istruzioni.
```cpp
Value* reachingDefinition = operand.get();

if (dyn_cast<Constant>(reachingDefinition) || dyn_cast<Argument>(reachingDefinition))
{
    //...
}
else if (Instruction* operandDefinitionInstr = dyn_cast<Instruction>(reachingDefinition))
{
    //...
}

```
Nella forma SSA le definizioni dominano gli usi, quindi per ottenere la reaching definition è sufficiente ottenere il Value che rappresenta quell'operando con `Value* Use::get()`.
Come possiamo effettuare il controllo sull'unicità della reaching definition? 
Supponiamo che vi siano due definizioni successive di una certa variabile utilizzata nell'istruzione per cui stiamo effettuando il controllo:
```
E = B + D
...
E = 5
...
// istruzione di riferimento
A = 2 * E
```
In questo caso la forma SSA risponde alla domanda assegnando una "versione" diversa alle due variabili:
```
E1 = B + D
...
E2 = 5
...
// istruzione di riferimento
A = 2 * E2
```
Supponiamo ora che vi siano due definizioni in branch di flusso separati:
```
if (...)
    E = 2
else
    E = 3
...
A = 2 * E
```
Nella forma SSA sarà presente un'istruzione PHI che rappresenta la dipendenza tra il flusso di controllo e le definizioni dell'operando: se
l'operando è una PHI, allora c'è sicuramente più di una reaching definition per quell'operando.\
Una PHI eredita da `Instruction`, ma nell'algoritmo fin qui descritto le uniche `Instruction` che possono essere marcate loop invariant sono i binary operators;
non si avrà mai una PHI marcata, perciò queste non si troveranno nel set `loopInvariantInstructions` e di conseguenza tutte le `Instruction` che ne fanno uso non saranno loop invariant.\

La verifica della presenza o meno della reaching definition all'interno del loop viene effettuata tramite il metodo della classe Loop `currentLoop->contains(operandDefinitionInstr)`.
```cpp
else if (Instruction* operandDefinitionInstr = dyn_cast<Instruction>(reachingDefinition))
{
     return !this->currentLoop->contains(operandDefinitionInstr) || this->loopInvariantInstructions.contains(operandDefinitionInstr);
}
```
# Individuazione istruzioni hoistable
Ora che l'analisi sulle istruzioni loop invariant ha prodotto il suo risultato, procediamo a determinare quali di queste possano essere effettivamente 
spostate nel blocco preheader.\
Un'istruzione è **hoistable** se _domina tutte le uscite del loop_. Abbiamo già un set in cui sono presenti gli _exiting blocks_; utilizziamo il risultato
del passo di analisi sulla dominanza: se l'istruzione domina tutti questi blocchi, allora è hoistable.
```cpp
bool LoopInvariantCodeMotionPass::dominatesAllExits(const Instruction* instr)
{
    return all_of(this->loopExitingBlocks, [this, &instr](BasicBlock* exitingBlock) {
        return this->dominatorTree->dominates(instr, exitingBlock);
    });
}
```
Esiste però anche una condizione meno stringente che può rendere un'istruzione hoistable: se il risultato dell'istruzione _non è più utilizzato dopo il loop_, ossia
l'istruzione è **dead** dopo il loop, allora l'istruzione è comunque hoistable.\

Possiamo verificare la condizione di _liveness_ di un'istruzione controllando se esiste almeno un suo uso esterno al loop.\

Tutti i Value che sono User di un'istruzione dovrebbero essere Instruction a loro volta; viene effettuato un controllo per verificare che sia quello il caso.
```cpp
bool LoopInvariantCodeMotionPass::isDeadAfterLoop(const Instruction* instr)
{
    return all_of(instr->uses(), [this](const Use& use) {
        Instruction* user = dyn_cast<Instruction>(use.getUser());

        if (user == nullptr)
        {
            //...

            return false;
        }
        else
        {
            return this->currentLoop->contains(user);
        }
    });
}
```
Il set di istruzioni `hoistableInstructions` contiene tutte le istruzioni da muovere nel blocco preheader.
```cpp
void LoopInvariantCodeMotionPass::searchForHoistableInstructions()
{
    auto hoistableInstructionsRange = make_filter_range(this->loopInvariantInstructions, [this](Instruction* instr) {
        return dominatesAllExits(instr) || isDeadAfterLoop(instr);
    });

    this->hoistableInstructions.insert(hoistableInstructionsRange.begin(), hoistableInstructionsRange.end());
}
```
# Esecuzione code motion
Ora è possibile semplicemente muovere le istruzioni nel preheader; viene usata la classe helper fornita da LLVM per operare sulla IR: `IRBuilder`.
```cpp
IRBuilder<> builder(preheader, preheader->begin());

for (auto* instr : hoistableInstructions)
{
    instr->removeFromParent();
        builder.Insert(instr);

    //...
}
//...
```
