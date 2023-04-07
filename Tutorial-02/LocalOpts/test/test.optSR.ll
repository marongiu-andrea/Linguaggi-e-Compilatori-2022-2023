; ModuleID = 'test/test.optSR.bc'
source_filename = "test/test.ll"

define dso_local void @foo(i32 noundef %0) {
  %2 = add nsw i32 %0, 0
  %3 = shl i32 %2, 4
  %4 = mul nsw i32 %2, 16
  %5 = mul nsw i32 %3, %2
  %6 = sdiv i32 %5, %0
  %7 = sdiv i32 %5, 10
  %8 = mul nsw i32 1, %6
  %9 = sdiv i32 %7, 128
  %10 = sdiv i32 %8, 54
  %11 = sdiv i32 %7, 1
  %12 = sub nsw i32 %10, 0
  ret void
}
