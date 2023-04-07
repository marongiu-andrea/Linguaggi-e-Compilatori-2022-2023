
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
  %2 = add nsw i32 %0, 0 ; algebraic identity
  %3 = mul nsw i32 %2, 16 ; strength reduction
  %4 = mul nsw i32 %3, %2
  %5 = sdiv i32 %4, %0
  %6 = sdiv i32 %4, 10
  %7 = mul nsw i32 1, %5
  %8 = sdiv i32 %6, 128 ; strength reduction
  %9 = sdiv i32 %7, 54
  %10 = sdiv i32 %6, 1 ; algebraic identity
  %11 = sub nsw i32 %9, 0 ; algebraic identity
  ; multi instruction optimization
  %12 = add nsw i32 %0, 10
  %13 = sub nsw i32 %12, 10
  ; ---
  %14 = sub nsw i32 %13, 7 ; first operand should be replaced with %0
  %15 = mul nsw i32 %14, 9 ; strength reduction (with add)
  %16 = mul nsw i32 %15, 7 ; strength reduction (with sub)
  %17 = add nsw i32 4, 0 ; algebraic identity with two constants
  %18 = sub nsw i32 %17, 5
  %19 = mul nsw i32 2, 8 ; strength reduction with two constants
  %20 = add nsw i32 %19, 3
  ret void
}

