; ModuleID = 'test/Loop.bc'
source_filename = "test/Loop.ll"

@.str = private constant [25 x i8] c"%d,%d,%d,%d,%d,%d,%d,%d\0A\00", align 1

define void @foo(i32 %0, i32 %1) {
  %3 = add nsw i32 %0, 3
  %4 = add nsw i32 %0, 7
  %5 = add nsw i32 %0, 4
  %6 = add nsw i32 %0, 4
  br label %7

7:                                                ; preds = %16, %2
  %.05 = phi i32 [ 0, %2 ], [ %20, %16 ]
  %.04 = phi i32 [ 0, %2 ], [ %18, %16 ]
  %.03 = phi i32 [ 0, %2 ], [ %17, %16 ]
  %.01 = phi i32 [ 9, %2 ], [ %.1, %16 ]
  %.0 = phi i32 [ %1, %2 ], [ %8, %16 ]
  %8 = add nsw i32 %.0, 1
  %9 = icmp slt i32 %8, 5
  br i1 %9, label %10, label %13

10:                                               ; preds = %7
  %11 = add nsw i32 %.01, 2
  %12 = add nsw i32 %0, 3
  br label %16

13:                                               ; preds = %7
  %14 = sub nsw i32 %.01, 1
  %15 = icmp sge i32 %8, 10
  br i1 %15, label %21, label %16

16:                                               ; preds = %13, %10
  %.02 = phi i32 [ %12, %10 ], [ %14, %13 ]
  %.1 = phi i32 [ %11, %10 ], [ %14, %13 ]
  %17 = add nsw i32 %3, 7
  %18 = add nsw i32 %.02, 2
  %19 = add nsw i32 %0, 7
  %20 = add nsw i32 %4, 5
  br label %7

21:                                               ; preds = %13
  %22 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([25 x i8], [25 x i8]* @.str, i64 0, i64 0), i32 %14, i32 %5, i32 %.03, i32 %.04, i32 %4, i32 %.05, i32 %3, i32 %8)
  ret void
}

declare i32 @printf(i8*, ...)

define i32 @main() {
  call void @foo(i32 0, i32 4)
  call void @foo(i32 0, i32 12)
  ret i32 0
}
