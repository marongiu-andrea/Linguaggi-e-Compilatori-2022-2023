; ModuleID = 'test/test.strengthreduction.optimized.bc'
source_filename = "test/test.ll"

define dso_local void @foo(i32 noundef %0) {
  %2 = add nsw i32 %0, 0
  %3 = mul nsw i32 %2, 16
  %4 = shl i32 %2, 4
  %5 = mul nsw i32 %4, %2
  %6 = sdiv i32 %5, %0
  %7 = sdiv i32 %5, 10
  %8 = mul nsw i32 1, %6
  %9 = shl i32 %6, 0
  %10 = sdiv i32 %7, 128
  %11 = sdiv i32 %9, 54
  %12 = sdiv i32 %7, 1
  %13 = sub nsw i32 %11, 0
  %14 = add nsw i32 %13, 1
  %15 = add nsw i32 %7, 5
  %16 = mul nsw i32 %11, 17
  %17 = shl i32 %11, 4
  %18 = add i32 %17, %11
  %19 = add nsw i32 %18, 10
  ret void
}
