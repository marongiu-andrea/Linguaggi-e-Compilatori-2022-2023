; ModuleID = 'Loops.opt.bc'
source_filename = "test/Loop.ll"

@.str = private constant [25 x i8] c"%d,%d,%d,%d,%d,%d,%d,%d\0A\00", align 1

define void @foo(i32 %0, i32 %1) {
  %3 = add nsw i32 %0, 3
  %4 = add nsw i32 %0, 7
  %5 = add nsw i32 %0, 4
  %6 = add nsw i32 %0, 3
  %7 = add nsw i32 %3, 7
  %8 = add nsw i32 %0, 7
  %9 = add nsw i32 %4, 5
  br label %10

10:                                               ; preds = %18, %2
  %.05 = phi i32 [ 0, %2 ], [ %9, %18 ]
  %.04 = phi i32 [ 0, %2 ], [ %19, %18 ]
  %.03 = phi i32 [ 0, %2 ], [ %7, %18 ]
  %.01 = phi i32 [ 9, %2 ], [ %.1, %18 ]
  %.0 = phi i32 [ %1, %2 ], [ %11, %18 ]
  %11 = add nsw i32 %.0, 1
  %12 = icmp slt i32 %11, 5
  br i1 %12, label %13, label %15

13:                                               ; preds = %10
  %14 = add nsw i32 %.01, 2
  br label %18

15:                                               ; preds = %10
  %16 = sub nsw i32 %.01, 1
  %17 = icmp sge i32 %11, 10
  br i1 %17, label %20, label %18

18:                                               ; preds = %15, %13
  %.02 = phi i32 [ %6, %13 ], [ %5, %15 ]
  %.1 = phi i32 [ %14, %13 ], [ %16, %15 ]
  %19 = add nsw i32 %.02, 2
  br label %10

20:                                               ; preds = %15
  %21 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([25 x i8], [25 x i8]* @.str, i64 0, i64 0), i32 %16, i32 %5, i32 %.03, i32 %.04, i32 %4, i32 %.05, i32 %3, i32 %11)
  ret void
}

declare i32 @printf(i8*, ...)

define i32 @main() {
  call void @foo(i32 0, i32 4)
  call void @foo(i32 0, i32 12)
  ret i32 0
}
