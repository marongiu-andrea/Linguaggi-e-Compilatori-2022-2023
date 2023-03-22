; ModuleID = './testTransform/FooOpt.bc'
source_filename = "./testTransform/Foo.ll"

define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) {
  %3 = mul nsw i32 %1, 1
  %4 = shl i32 %1, 1
  %5 = sub i32 %4, %1
  %6 = mul nsw i32 %5, 256
  %7 = shl i32 %0, 1
  %8 = mul i32 %7, 4
  %9 = mul nsw i32 %6, %8
  %10 = udiv i32 %9, 8
  %11 = lshr i32 %9, 3
  %12 = mul i32 %11, 15
  %13 = shl i32 %11, 4
  %14 = sub i32 %13, %11
  %15 = mul i32 15, %14
  %16 = shl i32 %14, 4
  %17 = sub i32 %16, %14
  ret i32 %17
}
