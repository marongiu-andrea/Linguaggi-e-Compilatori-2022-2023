define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) {
  %3 = add nsw i32 %1, 1
  %4 = mul nuw i32 %3, 2
  %5 = mul nsw i32 %3, 32
  %6 = mul i32 %3, 1024
  %7 = mul i32 %3, 33
  %8 = mul i32 %7, %6
  %9 = mul i32 %7, 31
  %10 = mul i32 %7, 1
  %11 = udiv i32 %10, 4
  %12 = udiv i32 %10, 5
  %13 = udiv i32 %10, 9
  %14 = udiv i32 %10, %12 
  ret i32 %10
}
