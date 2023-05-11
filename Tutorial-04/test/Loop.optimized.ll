; ModuleID = 'test/Loop.optimized.bc'
source_filename = "test/Loop.ll"

@.str = private constant [25 x i8] c"%d,%d,%d,%d,%d,%d,%d,%d\0A\00", align 1

define void @foo(i32 %0, i32 %1) {
  %3 = add nsw i32 %0, 3
  %4 = add nsw i32 %0, 7
  br label %5

5:                                                ; preds = %15, %2
  %.05 = phi i32 [ 0, %2 ], [ %19, %15 ]
  %.04 = phi i32 [ 0, %2 ], [ %17, %15 ]
  %.03 = phi i32 [ 0, %2 ], [ %16, %15 ]
  %.01 = phi i32 [ 9, %2 ], [ %.1, %15 ]
  %.0 = phi i32 [ %1, %2 ], [ %6, %15 ]
  %6 = add nsw i32 %.0, 1
  %7 = icmp slt i32 %6, 5
  br i1 %7, label %8, label %11

8:                                                ; preds = %5
  %9 = add nsw i32 %.01, 2
  %10 = add nsw i32 %0, 3
  br label %15

11:                                               ; preds = %5
  %12 = sub nsw i32 %.01, 1
  %13 = add nsw i32 %0, 4
  %14 = icmp sge i32 %6, 10
  br i1 %14, label %20, label %15

15:                                               ; preds = %11, %8
  %.02 = phi i32 [ %10, %8 ], [ %13, %11 ]
  %.1 = phi i32 [ %9, %8 ], [ %12, %11 ]
  %16 = add nsw i32 %3, 7
  %17 = add nsw i32 %.02, 2
  %18 = add nsw i32 %0, 7
  %19 = add nsw i32 %4, 5
  br label %5

20:                                               ; preds = %11
  %21 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([25 x i8], [25 x i8]* @.str, i64 0, i64 0), i32 %12, i32 %13, i32 %.03, i32 %.04, i32 %4, i32 %.05, i32 %3, i32 %6)
  ret void
}

declare i32 @printf(i8*, ...)

define i32 @main() {
  call void @foo(i32 0, i32 4)
  call void @foo(i32 0, i32 12)
  ret i32 0
}
