; ModuleID = 'testTransform/FooOpt.bc'
source_filename = "./testTransform/Foo.ll"

define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) {
  %3 = add nsw i32 %1, 1
  %4 = mul nsw i32 %3, 2
  %5 = shl i32 %0, 1
  %6 = sdiv i32 %5, 4
  %7 = mul nsw i32 %4, %6
  %8 = udiv i32 %7, 8
  %9 = mul i32 %8, 15
  %10 = mul i32 15, %9
  %11 = add i32 %10, 1
  %12 = sub i32 %11, 1
  %13 = add nsw i32 %5, %11
  ret i32 %10
}
