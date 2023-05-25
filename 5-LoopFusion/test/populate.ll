; ModuleID = 'test/populate.base.bc'
source_filename = "srcTest/populate.cc"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z8populatePiS_S_(i32* noundef %0, i32* noundef %1, i32* noundef %2) #0 {
  br label %4

4:                                                ; preds = %27, %3
  %.0 = phi i32 [ 0, %3 ], [ %28, %27 ]
  %5 = icmp slt i32 %.0, 100000
  br i1 %5, label %6, label %29

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
  %28 = add nsw i32 %.0, 1
  br label %4, !llvm.loop !6

29:                                               ; preds = %4
  br label %30

30:                                               ; preds = %44, %29
  %.1 = phi i32 [ 0, %29 ], [ %45, %44 ]
  %31 = icmp slt i32 %.1, 100000
  br i1 %31, label %32, label %46

32:                                               ; preds = %30
  %33 = sext i32 %.1 to i64
  %34 = getelementptr inbounds i32, i32* %0, i64 %33
  %35 = load i32, i32* %34, align 4
  %36 = sub nsw i32 100000, %.1
  %37 = sub nsw i32 %36, 1
  %38 = sext i32 %37 to i64
  %39 = getelementptr inbounds i32, i32* %2, i64 %38
  %40 = load i32, i32* %39, align 4
  %41 = add nsw i32 %35, %40
  %42 = sext i32 %.1 to i64
  %43 = getelementptr inbounds i32, i32* %1, i64 %42
  store i32 %41, i32* %43, align 4
  br label %44

44:                                               ; preds = %32
  %45 = add nsw i32 %.1, 1
  br label %30, !llvm.loop !8

46:                                               ; preds = %30
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
