; ModuleID = 'test/test.ll'
source_filename = "test/test.ll"

define dso_local void @foo(i32 noundef %0) {
  %2 = add nsw i32 %0, 0
  %3 = shl i32 %2, 4
  %4 = sub i32 %3, %2
  %5 = mul nsw i32 %4, %2
  %6 = sdiv i32 %5, %0
  %7 = sdiv i32 %5, 10
  %8 = shl i32 %6, 0
  %9 = lshr i32 %7, 7
  %10 = sdiv i32 %8, 54
  %11 = sdiv i32 %7, 1
  %12 = sub nsw i32 %10, 0
  %13 = sdiv i32 %12, 13
  %14 = mul nsw i32 13, %13
  %15 = add nsw i32 %14, 25
  ret void
}
