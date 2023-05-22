; ModuleID = './Realout.bc'
source_filename = "Loop.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @populate(i32* noundef %0, i32* noundef %1, i32* noundef %2, i32 noundef %3, i32 noundef %4) #0 {
  br label %6

6:                                                ; preds = %15, %5
  %.0 = phi i32 [ 0, %5 ], [ %16, %15 ]
  %7 = icmp slt i32 %.0, %3
  br i1 %7, label %8, label %32

8:                                                ; preds = %6
  %9 = sext i32 %.0 to i64
  %10 = getelementptr inbounds i32, i32* %2, i64 %9
  %11 = load i32, i32* %10, align 4
  %12 = mul nsw i32 5, %11
  %13 = sext i32 %.0 to i64
  %14 = getelementptr inbounds i32, i32* %0, i64 %13
  store i32 %12, i32* %14, align 4
  br label %20

15:                                               ; preds = %20
  %16 = add nsw i32 %.0, %4
  br label %6, !llvm.loop !4

17:                                               ; No predecessors!
  br label %18

18:                                               ; preds = %30, %17
  %19 = icmp slt i32 %.0, %3
  br i1 %19, label %20, label %32

20:                                               ; preds = %8, %18
  %21 = sext i32 %.0 to i64
  %22 = getelementptr inbounds i32, i32* %0, i64 %21
  %23 = load i32, i32* %22, align 4
  %24 = sext i32 %.0 to i64
  %25 = getelementptr inbounds i32, i32* %2, i64 %24
  %26 = load i32, i32* %25, align 4
  %27 = add nsw i32 %23, %26
  %28 = sext i32 %.0 to i64
  %29 = getelementptr inbounds i32, i32* %1, i64 %28
  store i32 %27, i32* %29, align 4
  br label %15

30:                                               ; No predecessors!
  %31 = add nsw i32 %.0, %4
  br label %18, !llvm.loop !6

32:                                               ; preds = %6, %18
  ret void
}

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"uwtable", i32 1}
!2 = !{i32 7, !"frame-pointer", i32 2}
!3 = !{!"clang version 14.0.5 (Fedora 14.0.5-2.fc36)"}
!4 = distinct !{!4, !5}
!5 = !{!"llvm.loop.mustprogress"}
!6 = distinct !{!6, !5}
