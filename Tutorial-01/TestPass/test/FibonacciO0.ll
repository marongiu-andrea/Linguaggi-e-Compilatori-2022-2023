; ModuleID = 'Fibonacci.c'
source_filename = "Fibonacci.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

%struct.__va_list_tag = type { i32, i32, ptr, ptr }

@stdout = external dso_local global ptr, align 8
@.str = private unnamed_addr constant [9 x i8] c"f(0) = 0\00", align 1
@.str.1 = private unnamed_addr constant [9 x i8] c"f(1) = 1\00", align 1
@.str.2 = private unnamed_addr constant [22 x i8] c"f(%d) = f(%d) + f(%d)\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @printf(ptr noundef %0, ...) #0 {
  %2 = alloca ptr, align 8
  %3 = alloca i32, align 4
  %4 = alloca [1 x %struct.__va_list_tag], align 16
  store ptr %0, ptr %2, align 8
  %5 = getelementptr inbounds [1 x %struct.__va_list_tag], ptr %4, i64 0, i64 0
  call void @llvm.va_start(ptr %5)
  %6 = load ptr, ptr @stdout, align 8
  %7 = load ptr, ptr %2, align 8
  %8 = getelementptr inbounds [1 x %struct.__va_list_tag], ptr %4, i64 0, i64 0
  %9 = call i32 @vfprintf(ptr noundef %6, ptr noundef %7, ptr noundef %8)
  store i32 %9, ptr %3, align 4
  %10 = getelementptr inbounds [1 x %struct.__va_list_tag], ptr %4, i64 0, i64 0
  call void @llvm.va_end(ptr %10)
  %11 = load i32, ptr %3, align 4
  ret i32 %11
}

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.va_start(ptr) #1

declare dso_local i32 @vfprintf(ptr noundef, ptr noundef, ptr noundef) #2

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.va_end(ptr) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @Fibonacci(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  %4 = load i32, ptr %3, align 4
  %5 = icmp eq i32 %4, 0
  br i1 %5, label %6, label %8

6:                                                ; preds = %1
  %7 = call i32 (ptr, ...) @printf(ptr noundef @.str)
  store i32 0, ptr %2, align 4
  br label %27

8:                                                ; preds = %1
  %9 = load i32, ptr %3, align 4
  %10 = icmp eq i32 %9, 1
  br i1 %10, label %11, label %13

11:                                               ; preds = %8
  %12 = call i32 (ptr, ...) @printf(ptr noundef @.str.1)
  store i32 1, ptr %2, align 4
  br label %27

13:                                               ; preds = %8
  %14 = load i32, ptr %3, align 4
  %15 = load i32, ptr %3, align 4
  %16 = sub nsw i32 %15, 1
  %17 = load i32, ptr %3, align 4
  %18 = sub nsw i32 %17, 2
  %19 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, i32 noundef %14, i32 noundef %16, i32 noundef %18)
  %20 = load i32, ptr %3, align 4
  %21 = sub nsw i32 %20, 1
  %22 = call i32 @Fibonacci(i32 noundef %21)
  %23 = load i32, ptr %3, align 4
  %24 = sub nsw i32 %23, 2
  %25 = call i32 @Fibonacci(i32 noundef %24)
  %26 = add nsw i32 %22, %25
  store i32 %26, ptr %2, align 4
  br label %27

27:                                               ; preds = %13, %11, %6
  %28 = load i32, ptr %2, align 4
  ret i32 %28
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind willreturn }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"uwtable", i32 2}
!2 = !{i32 7, !"frame-pointer", i32 2}
!3 = !{!"clang version 15.0.7 (Fedora 15.0.7-1.fc37)"}
