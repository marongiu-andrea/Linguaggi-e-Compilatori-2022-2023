; ModuleID = 'testTransform/Foo.optimized.bc'
source_filename = "testTransform/Foo.ll"

define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) {
  %3 = add i32 %1, %1
  %4 = mul nsw i32 %3, 2
  %5 = shl i32 %0, 1
  %6 = sdiv i32 %5, 4
  %7 = mul nsw i32 %4, %6
  %8 = mul nsw i32 15, %6
  %9 = add nsw i32 1, %3
  %10 = add nsw i32 %3, 2
  ret i32 %3
}
