; #include <stdio.h>

; void foo(int c, int z) {
;   int a = 9, h, m = 0, n = 0, q, r = 0, y = 0;

; LOOP:
;   z = z + 1;
;   y = c + 3;
;   q = c + 7;
;   if (z < 5) {
;     a = a + 2;
;     h = c + 3;
;   } else {
;     a = a - 1;
;     h = c + 4;
;     if (z >= 10) {
;       goto EXIT;
;     }
;   }
;   m = y + 7;
;   n = h + 2;
;   y = c + 7;
;   r = q + 5;
;   goto LOOP;
; EXIT:
;   printf("%d,%d,%d,%d,%d,%d,%d,%d\n", a, h, m, n, q, r, y, z);
; }

; int main() {
;   foo(0, 4);
;   foo(0, 12);
;   return 0;
; }
@.str = private constant [25 x i8] c"%d,%d,%d,%d,%d,%d,%d,%d\0A\00", align 1

define void @foo(i32 %0, i32 %1) {
  br label %3

3:                                                ; preds = %15, %2
  %.05 = phi i32 [ 0, %2 ], [ %20, %16 ]
  %.04 = phi i32 [ 0, %2 ], [ %18, %16 ]
  %.03 = phi i32 [ 0, %2 ], [ %17, %16 ]
  %.01 = phi i32 [ 9, %2 ], [ %.1, %16 ]
  %.0 = phi i32 [ %1, %2 ], [ %4, %16 ]
  %4 = add nsw i32 %.0, 1
  %5 = add nsw i32 %0, 3
  %6 = add nsw i32 %0, 7
  %7 = icmp slt i32 %4, 5
  br i1 %7, label %8, label %11

8:                                                ; preds = %3
  %9 = add nsw i32 %.01, 2
  %10 = add nsw i32 %0, 3
  br label %16

11:                                               ; preds = %3
  %12 = sub nsw i32 %.01, 1
  %13 = add nsw i32 %0, 4
  %14 = icmp sge i32 %4, 10
  %15 = add nsw i32 %0, 4
  br i1 %14, label %21, label %16

16:                                               ; preds = %11, %8
  %.02 = phi i32 [ %10, %8 ], [ %12, %11 ]
  %.1 = phi i32 [ %9, %8 ], [ %12, %11 ]
  %17 = add nsw i32 %5, 7
  %18 = add nsw i32 %.02, 2
  %19 = add nsw i32 %0, 7
  %20 = add nsw i32 %6, 5
  br label %3

21:                                               ; preds = %11
  %22 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([25 x i8], [25 x i8]* @.str, i64 0, i64 0), i32 %12, i32 %13, i32 %.03, i32 %.04, i32 %6, i32 %.05, i32 %5, i32 %4)
  ret void
}

declare i32 @printf(i8*, ...)

define i32 @main() {
  call void @foo(i32 0, i32 4)
  call void @foo(i32 0, i32 12)
  ret i32 0
}
