; ModuleID = 'test/test.multiinstructionoptimization.optimized.bc'
source_filename = "test/testMultiInstruction.ll"

define dso_local void @foo(i32 noundef %0) {
  %2 = sub nsw i32 %0, 1
  %3 = add nsw i32 %2, 1
  %4 = add nsw i32 %0, 10
  ret void
}
