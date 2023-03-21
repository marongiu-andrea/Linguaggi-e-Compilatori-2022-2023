; ModuleID = 'testTransform/Foo.bc'
source_filename = "testTransform/Foo.ll"

define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) {
  %3 = add nsw i32 %1, 1
  %4 = sdiv i32 %3, 1
  %5 = sdiv i32 %4, 8
  %6 = ashr i32 %4, 3
  %7 = mul nsw i32 7, %3
  %8 = shl i32 %3, 3
  %9 = sub i32 %8, %3
  %10 = sub nsw i32 %8, 2
  ret i32 %10
}
