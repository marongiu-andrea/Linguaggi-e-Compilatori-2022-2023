; ModuleID = './testTransform/FooOpt.bc'
source_filename = "./testTransform/Foo.ll"

define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) {
  %3 = mul nsw i32 %1, 1
  %4 = shl i32 %1, 0
  %5 = mul nsw i32 %4, 256
  %6 = shl i32 %4, 8
  %7 = shl i32 %0, 1
  %8 = mul i32 %7, 4
  %9 = shl i32 %7, 2
  %10 = mul nsw i32 %6, %9
  ret i32 %10
}
