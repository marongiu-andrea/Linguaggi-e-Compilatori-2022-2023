; ModuleID = 'testTransform/Foo.optimized.bc'
source_filename = "testTransform/Foo.ll"

define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) {
  %3 = add nsw i32 %1, 1
  %4 = add i32 %1, %1
  %5 = shl i32 %4, 1
  %6 = mul nsw i32 %4, 2
  %7 = shl i32 %0, 1
  %8 = sdiv i32 %7, 4
  %9 = mul nsw i32 %5, %8
  ret i32 %9
}
