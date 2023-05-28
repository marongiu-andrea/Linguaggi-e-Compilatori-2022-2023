plugin=./LoopFusion.so
passes=loop-fusion
optimizedbc=test/Loop.optimized.bc
optimizedll=test/Loop.optimized.ll
opt -load-pass-plugin=$plugin -passes=$passes test/Loop.ll -o $optimizedbc
llvm-dis $optimizedbc -o $optimizedll