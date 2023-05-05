; ModuleID = 'test/test.ll'
source_filename = "test/test.ll"

define dso_local void @foo(i32 noundef %0) {
  %2 = mul nsw i32 %0, 15
  %3 = mul nsw i32 %2, %0
  %4 = sdiv i32 %3, %0
  %5 = sdiv i32 %3, 10
  %6 = sdiv i32 %5, 128
  %7 = sdiv i32 %4, 54
  %8 = sdiv i32 %7, 13
  %9 = mul nsw i32 13, %8
  %10 = add nsw i32 %9, 25
  ret void
}
