; ModuleID = 'testTransform/foo.ll'
source_filename = "testTransform/foo.ll"

define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) {
  %3 = add nsw i32 %1, 1
  %4 = mul nsw i32 %3, 2
  %5 = shl i32 %3, 1
  %6 = shl i32 %4, 1
  %7 = sdiv i32 %6, 4
  %8 = mul nsw i32 %5, %7
  ret i32 %4
}
