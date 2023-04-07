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
  %11 = lshr i32 %7, 7
  %12 = sdiv i32 %9, 54
  %13 = sdiv i32 %7, 1
  %14 = lshr i32 %7, 0
  %15 = sub nsw i32 %12, 0
  %16 = add nsw i32 %15, 1
  %17 = add nsw i32 %14, 5
  %18 = mul nsw i32 %12, 17
  %19 = shl i32 %12, 4
  %20 = add i32 %19, %12
  %21 = add nsw i32 %20, 10
  ret void
}
