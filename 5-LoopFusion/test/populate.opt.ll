; ModuleID = 'test/populate.opt.bc'
source_filename = "srcTest/populate.cc"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z8populatePiS_S_(i32* noundef %0, i32* noundef %1, i32* noundef %2) #0 {
  br label %4

4:                                                ; preds = %39, %3
  %.0 = phi i32 [ 0, %3 ], [ %40, %39 ]
  %5 = icmp slt i32 %.0, 100000
  br i1 %5, label %6, label %41

6:                                                ; preds = %4
  %7 = sub nsw i32 100000, %.0
  %8 = sub nsw i32 %7, 1
  %9 = sext i32 %8 to i64
  %10 = getelementptr inbounds i32, i32* %2, i64 %9
  %11 = load i32, i32* %10, align 4
  %12 = mul nsw i32 5, %11
  %13 = sext i32 %.0 to i64
  %14 = getelementptr inbounds i32, i32* %0, i64 %13
  store i32 %12, i32* %14, align 4
  %15 = sext i32 %.0 to i64
  %16 = getelementptr inbounds i32, i32* %0, i64 %15
  %17 = load i32, i32* %16, align 4
  %18 = icmp sgt i32 %17, 10
  br i1 %18, label %19, label %26

19:                                               ; preds = %6
  %20 = sext i32 %.0 to i64
  %21 = getelementptr inbounds i32, i32* %2, i64 %20
  %22 = load i32, i32* %21, align 4
  %23 = mul nsw i32 5, %22
  %24 = sext i32 %.0 to i64
  %25 = getelementptr inbounds i32, i32* %1, i64 %24
  store i32 %23, i32* %25, align 4
  br label %26

26:                                               ; preds = %19, %6
  br label %27

27:                                               ; preds = %26
  %28 = sext i32 %.0 to i64
  %29 = getelementptr inbounds i32, i32* %0, i64 %28
  %30 = load i32, i32* %29, align 4
  %31 = sub nsw i32 100000, %.0
  %32 = sub nsw i32 %31, 1
  %33 = sext i32 %32 to i64
  %34 = getelementptr inbounds i32, i32* %2, i64 %33
  %35 = load i32, i32* %34, align 4
  %36 = add nsw i32 %30, %35
  %37 = sext i32 %.0 to i64
  %38 = getelementptr inbounds i32, i32* %1, i64 %37
  store i32 %36, i32* %38, align 4
  br label %39

39:                                               ; preds = %27
  %40 = add nsw i32 %.0, 1
  br label %4, !llvm.loop !6

41:                                               ; preds = %4
  br label %42

42:                                               ; preds = %44, %41
  %.1 = phi i32 [ 0, %41 ], [ %45, %44 ]
  %43 = icmp slt i32 %.0, 100000
  br i1 %43, label %.split, label %46

.split:                                           ; preds = %42
  br label %44

44:                                               ; preds = %.split
  %45 = add nsw i32 %.0, 1
  br label %42, !llvm.loop !8

46:                                               ; preds = %42
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
