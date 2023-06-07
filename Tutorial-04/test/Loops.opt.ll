; ModuleID = 'Loops.opt.ll'
source_filename = "test/Loops.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.timespec = type { i64, i64 }

@.str = private unnamed_addr constant [20 x i8] c"Time elapsed: %Lf\0A\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local void @populate(i32* noundef %0, i32* noundef %1, i32* noundef %2) #0 {
  br label %4

4:                                                ; preds = %27, %3
  %.0 = phi i32 [ 0, %3 ], [ %28, %27 ]
  %5 = icmp slt i32 %.0, 40000
  br i1 %5, label %6, label %29

6:                                                ; preds = %4
  %7 = sub nsw i32 40000, %.0
  %8 = sub nsw i32 %7, 1
  %9 = sext i32 %8 to i64
  %10 = getelementptr inbounds i32, i32* %2, i64 %9
  %11 = load i32, i32* %10, align 4
  %12 = mul nsw i32 5, %11
  %13 = sext i32 %.0 to i64
  %14 = getelementptr inbounds i32, i32* %0, i64 %13
  store i32 %12, i32* %14, align 4
  br label %15

15:                                               ; preds = %6
  %16 = sext i32 %.0 to i64
  %17 = getelementptr inbounds i32, i32* %0, i64 %16
  %18 = load i32, i32* %17, align 4
  %19 = sub nsw i32 40000, %.0
  %20 = sub nsw i32 %19, 1
  %21 = sext i32 %20 to i64
  %22 = getelementptr inbounds i32, i32* %2, i64 %21
  %23 = load i32, i32* %22, align 4
  %24 = add nsw i32 %18, %23
  %25 = sext i32 %.0 to i64
  %26 = getelementptr inbounds i32, i32* %1, i64 %25
  store i32 %24, i32* %26, align 4
  br label %27

27:                                               ; preds = %15
  %28 = add nsw i32 %.0, 1
  br label %4, !llvm.loop !6

29:                                               ; preds = %4
  br label %30

30:                                               ; preds = %32, %29
  %.1 = phi i32 [ 0, %29 ], [ %33, %32 ]
  %31 = icmp slt i32 %.0, 40000
  br i1 %31, label %.split, label %34

.split:                                           ; preds = %30
  br label %32

32:                                               ; preds = %.split
  %33 = add nsw i32 %.0, 1
  br label %30, !llvm.loop !8

34:                                               ; preds = %30
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [40000 x i32], align 16
  %2 = alloca [40000 x i32], align 16
  %3 = alloca [40000 x i32], align 16
  %4 = alloca %struct.timespec, align 8
  %5 = alloca %struct.timespec, align 8
  br label %6

6:                                                ; preds = %11, %0
  %.01 = phi i32 [ 0, %0 ], [ %12, %11 ]
  %7 = icmp slt i32 %.01, 40000
  br i1 %7, label %8, label %13

8:                                                ; preds = %6
  %9 = sext i32 %.01 to i64
  %10 = getelementptr inbounds [40000 x i32], [40000 x i32]* %3, i64 0, i64 %9
  store i32 1, i32* %10, align 4
  br label %11

11:                                               ; preds = %8
  %12 = add nsw i32 %.01, 1
  br label %6, !llvm.loop !9

13:                                               ; preds = %6
  %14 = call i32 @clock_gettime(i32 noundef 0, %struct.timespec* noundef %4) #3
  br label %15

15:                                               ; preds = %21, %13
  %.0 = phi i32 [ 0, %13 ], [ %22, %21 ]
  %16 = icmp slt i32 %.0, 40000
  br i1 %16, label %17, label %23

17:                                               ; preds = %15
  %18 = getelementptr inbounds [40000 x i32], [40000 x i32]* %1, i64 0, i64 0
  %19 = getelementptr inbounds [40000 x i32], [40000 x i32]* %2, i64 0, i64 0
  %20 = getelementptr inbounds [40000 x i32], [40000 x i32]* %3, i64 0, i64 0
  call void @populate(i32* noundef %18, i32* noundef %19, i32* noundef %20)
  br label %21

21:                                               ; preds = %17
  %22 = add nsw i32 %.0, 1
  br label %15, !llvm.loop !10

23:                                               ; preds = %15
  %24 = call i32 @clock_gettime(i32 noundef 0, %struct.timespec* noundef %5) #3
  %25 = getelementptr inbounds %struct.timespec, %struct.timespec* %5, i32 0, i32 0
  %26 = load i64, i64* %25, align 8
  %27 = getelementptr inbounds %struct.timespec, %struct.timespec* %4, i32 0, i32 0
  %28 = load i64, i64* %27, align 8
  %29 = sub nsw i64 %26, %28
  %30 = sitofp i64 %29 to x86_fp80
  %31 = getelementptr inbounds %struct.timespec, %struct.timespec* %5, i32 0, i32 1
  %32 = load i64, i64* %31, align 8
  %33 = getelementptr inbounds %struct.timespec, %struct.timespec* %4, i32 0, i32 1
  %34 = load i64, i64* %33, align 8
  %35 = sub nsw i64 %32, %34
  %36 = sitofp i64 %35 to x86_fp80
  %37 = fdiv x86_fp80 %36, 0xK401CEE6B280000000000
  %38 = fadd x86_fp80 %30, %37
  %39 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([20 x i8], [20 x i8]* @.str, i64 0, i64 0), x86_fp80 noundef %38)
  ret i32 0
}

; Function Attrs: nounwind
declare i32 @clock_gettime(i32 noundef, %struct.timespec* noundef) #1

declare i32 @printf(i8* noundef, ...) #2

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nounwind }

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
!9 = distinct !{!9, !7}
!10 = distinct !{!10, !7}
