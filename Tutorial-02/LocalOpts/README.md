# Istruzioni per testare i passi di ottimizzazione

In questo laboratorio sono stati implementati 3 passi:
- `algebraic-identity`;
- `strength-reduction`;
- `multi-instr-opt`

Per testare questi passi:

1. clonare il repository;
2. generare il file `libLocalOpts.so` tramite `make all` (comando da dare all'interno di questa directory - `Tutorial-02/LocalOpts`);
3. eseguire il comando `opt -load-pass-plugin=./libLocalOpts.so -passes=${pass} -S test/test.ll -o ${outputFileName}` dove:
  - `${pass}` dev'essere sostituito con uno dei passi elencati sopra;
  - `${outputFileName}` dev'essere sostituito con il nome/percorso del file di output. Nota: grazie all'opzione `-S` passata ad `opt`, questo file sarà in LLVM IR e non in bitcode.