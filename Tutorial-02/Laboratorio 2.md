# Laboratorio 2

<aside>
📝 Gruppo: Gabriele Cusato, matricola 152669, e Alessandro Giacconi, matricola 152533.

</aside>

---

# Esercizio 1 – Studio del passo

### Confrontare Foo.ll e Foo.optimized.ll, e tramite lo studio del passo in Transform.cpp capire cosa fa (e come lo fa)

Foo.ll

```llvm
; int foo(int e, int a) {
;   int b = a + 1;
;   int c = b * 2;
;   b = e << 1;
;   int d = b / 4;
;   return c * d;
; }

define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) #0 {
  %3 = add nsw i32 %1, 1
  %4 = mul nsw i32 %3, 2
  %5 = shl i32 %0, 1
  %6 = sdiv i32 %5, 4
  %7 = mul nsw i32 %4, %6
  ret i32 %7
}
```

Foo.optimized.ll

```llvm
; ModuleID = 'testTransform/Foo.optimized.bc'
source_filename = "testTransform/Foo.ll"

define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) {
  %3 = add nsw i32 %1, 1
  %4 = add i32 %1, %1
  %5 = mul nsw i32 %4, 2
  %6 = shl i32 %0, 1
  %7 = sdiv i32 %6, 4
  %8 = mul nsw i32 %5, %7
  ret i32 %8
}
```

Il codice ottimizzato ha eliminato una variabile temporanea e ha ridotto il numero di istruzioni eseguite, potenzialmente migliorando leggermente le prestazioni dell'esecuzione del codice, inoltre nel modulo "Foo.optimized.ll" è stata slinkata la prima istruzione in modo che non venisse più utilizzata nel resto del basic block.

### Studiare la documentazione, rispondere alla domanda
presente nel commento verso la fine del passo
Transform.cpp → Si possono aggiornare le singole references separatamente?

Tramite un controllo della documentazione siamo venuti a conoscenza del metodo “replaceUsesOfWith”, ma per il suo corretto funzionamento bisogna ottenre il puntatore all’istruzione in cui si vuole effettuare la sostituzione e poi nelle parentesi specificare quale valore si vuole sostituire con cosa.

Presentiamo un esempio, eseguibile anche decomentando le opportune righe in Transform.ccp:

```cpp
auto lastInst = B.end(); // cerco di arrivare all'istruzione %7 = mul nsw i32 %4, %6
--lastInst; // return sono 2 istruzioni
--lastInst;
(*lastInst).replaceUsesOfWith((&*Iter), NewInst2); // sostituisco il %4 con %5
```

Quindi, “replaceAllUsesWith” sostituisce tutte le occorrenze del valore di origine con il valore di destinazione in tutto il modulo, invece “replaceUsesOfWith” sostituisce solo il riferimento specificato.

---

# Esercizio 2 – Un passo più utile

### Per esercitarci a manipolare la IR modifichiamo adesso il
passo Transform.cpp perché sostituisca tutte le operazioni di
moltiplicazione che hanno tra gli operandi una costante che
è una potenza di 2 con una shift (strength reduction)

Transform è stato modificato in modo che da un istruzione che utilizza la MUL che viene slinkata si generi una nuova istruzione SHL, o shift. Per una spiegazione più dettagliata fare riferimento ai commenti nel file.

---

# Esercizio 3 – Primo assignment

### Implementare tre passi LLVM che realizzano le seguenti ottimizzazioni locali:
### • 1. Algebraic Identity
### • 2. Strength Reduction (più avanzato)
### • 3. Multi-Instruction Optimization

Per ognuno dei 3 passi da svolgere è stato creato un nuovo file cpp all'interno nella cartella lib, descritto dagli opportuni commenti all'interno del codice. Inoltre sono state create le nuove classi nel file LocalOpts.h relative ad ogni passo ed infine nel file LocalOpts.cpp sono stati aggiunti gli if realtivi ai passi da svolgere.

il comando per testare il corretto funzionamento dei passi è il seguente:

```bash
opt -load-pass-plugin=./libLocalOpts.so -passes=<nome-passo> test/test.ll -S -o test/test.optimized.<nome-passo>.ll
```

Nella cartella test, sono già presenti i file di test generati in questo modo per ogni passo. Si può controllare la corretta esecuzione del passo confrontando il file test.ll e il file generato dal passo rispettivo per vedere cos'è successo.