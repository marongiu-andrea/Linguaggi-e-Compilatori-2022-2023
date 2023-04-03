#! /bin/bash

srcfile="testTransform/Foo.ll"          
outfile="testTransform/Foo.optimized.bc"
opt -load-pass-plugin=./libLocalOpts.so -passes=transform $srcfile -o $outfile 

srcfile="testTransform/Foo.optimized.bc"         
outfile="testTransform/Foo.optimized.ll"
llvm-dis $srcfile -o $outfile