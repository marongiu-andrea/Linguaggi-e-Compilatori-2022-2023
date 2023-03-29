
; #include <stdio.h>

; void foo(int a) {
;   int r0 = a + 10;
;   int r1 = r0 - 10;
;   int r2 = r1 * r0;
;   int r3 = r2 - 10;
;   int r4 = r3 + 10;
;   int r5 = r0 - 10;
;   int r6 = r5 + 8;
;   int r7 = r4 + 8;
; }





define dso_local void @foo(i32 noundef %0) {
  %2 = add nsw i32 %0, 10
  %3 = sub nsw i32 %2, 10
  %4 = mul nsw i32 %3, %2
  %5 = sub nsw i32 %4, 10
  %6 = add nsw i32 %5, 10
  %7 = sub nsw i32 %2, 10
  %8 = add nsw i32 %7, 8
  %9 = add nsw i32 %6, 8
  ret void
}

