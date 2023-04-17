
; #include <stdio.h>

; void foo(int a) {
;   int r0 = a + 0;
;   int r1 = r0 * 10;
;   int r2 = r1 * 15;
;   int r3 = r2 * 7;
;   int r4 = r3 / 5;
;   int r5 = r4 / 16;
; }


define dso_local void @foo(i32 noundef %0) {
  %2 = add nsw i32 %0, 0
  %3 = mul nsw i32 %2, 10
  %4 = mul nsw i32 %3, 15
  %5 = mul nsw i32 %4, 7
  %6 = sdiv i32 %5, 5
  %7 = sdiv i32 %6, 16
  ret void
}

