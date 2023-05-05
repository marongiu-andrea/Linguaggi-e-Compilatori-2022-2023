# Terzo Assignment: Loop Invariant Code Motion
L'algoritmo di ottimizzazione Loop Invariant Code Motion si divide in 4 passi:
	+ recupero informazioni di analisi
	+ individuazione delle istruzioni loop invariant
	+ individuazione istruzioni safe-to-move
	+ esecuzione code motion
Ogni passo verrà discusso di seguito.

## Recupero informazioni
## Individuazione istruzioni loop invariant
Una istruzione del tipo **A = B + C** è loop invariant se:
- Le reaching definitions dei suoi operandi si trovano fuori dal loop;
- ogni operando ha esattmente una reaching definition all'interno del loop che è già stata marcata loop invariant.

Iniziamo esaminando tutte le istruzioni del Loop: consideriamo in prima analisi gli operatori binari controllando il flag `isBinaryOp()`
```cpp
for(auto& bb: L->getBlocks())
{
	for(auto& i: *bb)
	{
		if(i.isBnaryOp())
		{
			//...
		}
	}
}
```
Per ottenere le definizioni degli operandi, ossia la use-def chain, si utilizza `operands()`:
```cpp
loop_inv = true;
for(Use& u: i.operands())
{
	Value *v = u.get();
	// ...
}
if(loop_inv)
	liiSet.insert(&i);
```
Ora occorre controllare le condizioni. Occorre lavorare solo sulle Instruction, tutti gli altri Value sono loop-invariant per costruzione
Per controllare se la definizione si trova fuori dal loop, utilizzo il metodo Loop::constains(Instruction* I);
se questo confornto fallisce la reaching definition arriva da fuori, altrimenti è interna al loop e quindi scatta il controllo per la seconda condizione.
Per questa seconda condizione è sufficiente accertarsi che la reaching definition sia già stata marcata loop invariant:
ciò che comunica la presenza di più reaching definitions è l'avere come uso un'istruzione PHI; siccome le istruzioni PHI non possono essere marcate loop invariant,
basta effetturare il singolo controllo sulla marcatura.
E' sufficiente che un solo degli operandi non sia conforme che tutta l'istruzione non può essere loop invariant
```cpp
if(Instruction *I = dyn_cast<Instruction>(v))
{
	if(L->contains(I) && !liiSet.contains(I))
	{
		loop_inv = false;
		break;
	}
}
```
