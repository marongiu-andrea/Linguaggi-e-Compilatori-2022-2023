; ModuleID = 'Loop.bc'
source_filename = "test/Loop.c"
target datalayout = "e-m:e-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128"
target triple = "aarch64-unknown-linux-gnu"

; Function Attrs: noinline nounwind
define dso_local void @populate(i32* %0, i32* %1, i32* %2) #0 {
  br label %4

4:                                                ; preds = %13, %3
  %.0 = phi i32 [ 0, %3 ], [ %14, %13 ]
  %5 = icmp slt i32 %.0, 100
  br i1 %5, label %6, label %16

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
  br label %4

15:                                               ; No predecessors!
  br label %16

16:                                               ; preds = %4, %28, %15
  %17 = icmp slt i32 %.0, 100
  br i1 %17, label %18, label %30

18:                                               ; preds = %6, %16
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

28:                                               ; No predecessors!
  %29 = add nsw i32 %.0, 1
  br label %16

30:                                               ; preds = %16
  ret void
}

attributes #0 = { noinline nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="non-leaf" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Debian clang version 11.0.1-2"}
