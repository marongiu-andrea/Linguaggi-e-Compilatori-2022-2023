opt -load-pass-plugin=./libLocalOpts.so -passes=transform testTransform/Foo.ll -o testTransform/Foo.optimized.bc

llvm-dis-14 testTransform/Foo.opimized.bc -o testTransform/Foo.opimized.ll