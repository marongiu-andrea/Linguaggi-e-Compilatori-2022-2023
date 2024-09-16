
; #include <stdio.h>

; void foo(int a) {
;   int r0 = a + 0;
;   int r1 = r0 * 16;
;   int r2 = r1 * r0;
;   int r3 = r2 / a;
;   int r4 = r2 / 10;
;   int r5 = 1 * r3;
;   int r6 = r4 / 128;
;   int r7 = r5 / 54;
;   int r8 = r4 / 1;
;   int r9 = r7 - 0;
; }


define dso_local void @foo(i32 noundef %0) {
  %2 = add nsw i32 %0, 0    ;0
  %3 = mul nsw i32 %2, 16   ;1
  %4 = mul nsw i32 %3, %2   ;2
  %5 = sdiv i32 %4, %0      ;3
  %6 = sdiv i32 %4, 10      ;4
  %7 = mul nsw i32 1, %5    ;5
  %8 = sdiv i32 %6, 128     ;6
  %9 = sdiv i32 %7, 54      ;7
  %10 = sdiv i32 %6, 1      ;8
  %11 = sub nsw i32 %9, 0   ;9
  %12 = mul nsw i32 %3, 4   ;10
  %13 = add nsw i32 %4, 5   ;11
  %14 = sdiv i32 %12, 4  ;12
  %15 = add nsw i32 %14, 7  ;13
  ret void
}

