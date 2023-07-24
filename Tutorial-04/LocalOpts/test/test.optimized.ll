; ModuleID = 'test/Loop.opt.ll'
source_filename = "Loop.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z8populatePiS_S_(i32* noundef %0, i32* noundef %1, i32* noundef %2) #0 {
  br label %4

4:                                                ; preds = %13, %3
  %.0 = phi i32 [ 0, %3 ], [ %14, %13 ]
  %5 = icmp slt i32 %.0, 100
  br i1 %5, label %6, label %30

6:                                                ; preds = %4
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds i32, i32* %2, i64 %7
  %9 = load i32, i32* %8, align 4
  %10 = mul nsw i32 5, %9
  %11 = sext i32 %.0 to i64
  %12 = getelementptr inbounds i32, i32* %0, i64 %11
  store i32 %10, i32* %12, align 4
  br label %18

13:                                               ; preds = %18
  %14 = add nsw i32 %.0, 1
  br label %4, !llvm.loop !6

15:                                               ; No predecessors!
  br label %16

16:                                               ; preds = %28, %15
  %.1 = phi i32 [ 0, %15 ], [ %29, %28 ]
  %17 = icmp slt i32 %.0, 100
  br i1 %17, label %28, label %30

18:                                               ; preds = %6
  %19 = sext i32 %.0 to i64
  %20 = getelementptr inbounds i32, i32* %0, i64 %19
  %21 = load i32, i32* %20, align 4
  %22 = sext i32 %.0 to i64
  %23 = getelementptr inbounds i32, i32* %2, i64 %22
  %24 = load i32, i32* %23, align 4
  %25 = add nsw i32 %21, %24
  %26 = sext i32 %.0 to i64
  %27 = getelementptr inbounds i32, i32* %1, i64 %26
  store i32 %25, i32* %27, align 4
  br label %13

28:                                               ; preds = %16
  %29 = add nsw i32 %.0, 1
  br label %16, !llvm.loop !8

30:                                               ; preds = %4, %16
  ret void
}

attributes #0 = { mustprogress noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 14.0.0-1ubuntu1"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
!8 = distinct !{!8, !7}
