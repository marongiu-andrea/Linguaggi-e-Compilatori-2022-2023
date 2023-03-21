;int foo(int e, int a) {
;   int b = a + 1;
;   int c = b * 2;
;   b = e << 1;
;   int d = b / 4;
;   return c * d;
; }

define dso_local i32 @foo(i32 noundef %0, i32 noundef %1) #0 {
  %3 = add nsw i32 %1, 1
  %4 = sdiv i32 %3, 1
  %5 = sdiv i32 %4, 8
  %6 = mul nsw i32 7, %3
  %7 = sub nsw i32 %6, 2
  ret i32 %7
}