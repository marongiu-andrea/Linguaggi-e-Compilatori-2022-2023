opt -load-pass-plugin=./libLocalOpts.so -passes=LoopFusion test/Loop.bc -o test/Loop.opt.bc
llvm-dis test/Loop.opt.bc -o test/Loop.opt.ll
