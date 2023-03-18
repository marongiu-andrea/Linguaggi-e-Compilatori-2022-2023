; ModuleID = './testTransform/Foo.bc'
source_filename = "./testTransform/Foo.ll"

define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) {
  %3 = add nsw i32 %1, 1
  %4 = add i32 %1, %1
  %5 = mul nsw i32 %4, 2
  %6 = shl i32 %0, 1
  %7 = sdiv i32 %6, 4
  %8 = mul nsw i32 %5, %7
  ret i32 %8
}
