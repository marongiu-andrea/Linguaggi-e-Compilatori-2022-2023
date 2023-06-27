opt -load-pass-plugin=./libLocalOpts.so -passes=transform testTransform/Foo.ll -o testTransform/Foo.optimized.bc
llvm-dis testTransform/Foo.optimized.bc -o testTransform/Foo.optimized.ll