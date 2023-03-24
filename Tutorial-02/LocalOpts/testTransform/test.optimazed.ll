; ModuleID = 'testTransform/test.algebraic.bc'
source_filename = "test/test.ll"

define dso_local void @foo(i32 noundef %0) {
  %2 = mul nsw i32 %0, 16
  %3 = mul nsw i32 %2, %0
  %4 = sdiv i32 %3, %0
  %5 = sdiv i32 %3, 10
  %6 = sdiv i32 %5, 128
  %7 = sdiv i32 %4, 54
  ret void
}
