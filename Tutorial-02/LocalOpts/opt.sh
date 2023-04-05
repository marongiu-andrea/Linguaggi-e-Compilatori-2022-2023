#! /bin/bash

pass=$1

if [[ ! -z $pass ]]
then
  srcfile="testTransform/Foo.ll"          
  outfile="testTransform/Foo.optimized.bc"
  opt -load-pass-plugin=./libLocalOpts.so -passes=$pass $srcfile -o $outfile 
  
  srcfile="testTransform/Foo.optimized.bc"         
  outfile="testTransform/Foo.optimized.ll"
  llvm-dis $srcfile -o $outfile
else
  echo "[Error] You must set pass <algebric_identity, strength_reduction, multi_instruction>"
fi



