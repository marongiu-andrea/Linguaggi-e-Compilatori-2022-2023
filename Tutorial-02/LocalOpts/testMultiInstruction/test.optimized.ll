; ModuleID = 'testMultiInstruction/test.optimized.bc'
source_filename = "testMultiInstruction/test.ll"

define dso_local void @foo(i32 noundef %0) {
  %2 = add nsw i32 %0, 10
  %3 = mul nsw i32 %0, %2
  %4 = sub nsw i32 %3, 10
  %5 = add nsw i32 %0, 8
  %6 = add nsw i32 %3, 8
  ret void
}
