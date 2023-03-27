; ModuleID = 'testMultiInstruction/test.optimized.bc'
source_filename = "testMultiInstruction/test.ll"

define dso_local void @foo(i32 noundef %0) {
  %2 = add nsw i32 %0, 10
  %3 = sub nsw i32 %2, 10
  %4 = mul nsw i32 %3, %2
  %5 = sub nsw i32 %4, 10
  %6 = add nsw i32 %5, 10
  %7 = sub nsw i32 %2, 10
  ret void
}
