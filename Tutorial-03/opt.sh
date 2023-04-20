#! /bin/bash

pass="#" #$1

if [[ ! -z $pass ]]
then
  srcfile="test/Loop.ll"          
  opt -enable-new-pm=0 -load ./libLoopWalk.so -loop-walk $srcfile -disable-output
else
  echo "[Error] Invalid pass"
fi



