make
#OPTIMIZE FOR ALGEBRIC 
opt -load-pass-plugin=./libLocalOpts.so -passes=algebric test/test.ll -o test/test.opt.bc
llvm-dis test/test.opt.bc -o test/test.opt.ll
#OPTIMIZE FOR STRENGTH REDUCTION 
#opt -load-pass-plugin=./libLocalOpts.so -passes=strength_reduction test/test.ll -o test/test.opt.bc
#llvm-dis test/test.opt.bc -o test/test.opt.ll
#OPTIMIZE FOR MULTI INSTRUCTION 
#opt -load-pass-plugin=./libLocalOpts.so -passes=multi_instruction test/test.ll -o test/test.opt.bc
#llvm-dis test/test.opt.bc -o test/test.opt.ll
