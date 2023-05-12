# Generazione di un file di test

1. scrivere il file `.c`;
2. compilarlo con `clang -O0 -Xclang -disable-O0-optnone -emit-llvm -S -c <source file> -o <output file>`;
3. eseguire `opt -passes=mem2reg -S <source file> -o <output file>`