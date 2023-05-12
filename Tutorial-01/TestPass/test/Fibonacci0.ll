; ModuleID = 'Fibonacci.c'
source_filename = "Fibonacci.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.__va_list_tag = type { i32, i32, ptr, ptr }

@stdout = external global ptr, align 8
@.str = private unnamed_addr constant [9 x i8] c"f(0) = 0\00", align 1
@.str.1 = private unnamed_addr constant [9 x i8] c"f(1) = 1\00", align 1
@.str.2 = private unnamed_addr constant [22 x i8] c"f(%d) = f(%d) + f(%d)\00", align 1

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local i32 @printf(ptr noundef %0, ...) #0 !dbg !9 {
  %2 = alloca ptr, align 8
  %3 = alloca i32, align 4
  %4 = alloca [1 x %struct.__va_list_tag], align 16
  store ptr %0, ptr %2, align 8
  %5 = getelementptr inbounds [1 x %struct.__va_list_tag], ptr %4, i64 0, i64 0, !dbg !12
  call void @llvm.va_start(ptr %5), !dbg !12
  %6 = load ptr, ptr @stdout, align 8, !dbg !13
  %7 = load ptr, ptr %2, align 8, !dbg !14
  %8 = getelementptr inbounds [1 x %struct.__va_list_tag], ptr %4, i64 0, i64 0, !dbg !15
  %9 = call i32 @vfprintf(ptr noundef %6, ptr noundef %7, ptr noundef %8), !dbg !16
  store i32 %9, ptr %3, align 4, !dbg !17
  %10 = getelementptr inbounds [1 x %struct.__va_list_tag], ptr %4, i64 0, i64 0, !dbg !18
  call void @llvm.va_end(ptr %10), !dbg !18
  %11 = load i32, ptr %3, align 4, !dbg !19
  ret i32 %11, !dbg !20
}

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.va_start(ptr) #1

declare i32 @vfprintf(ptr noundef, ptr noundef, ptr noundef) #2

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.va_end(ptr) #1

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local i32 @Fibonacci(i32 noundef %0) #0 !dbg !21 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  %4 = load i32, ptr %3, align 4, !dbg !22
  %5 = icmp eq i32 %4, 0, !dbg !23
  br i1 %5, label %6, label %8, !dbg !22

6:                                                ; preds = %1
  %7 = call i32 (ptr, ...) @printf(ptr noundef @.str), !dbg !24
  store i32 0, ptr %2, align 4, !dbg !25
  br label %27, !dbg !25

8:                                                ; preds = %1
  %9 = load i32, ptr %3, align 4, !dbg !26
  %10 = icmp eq i32 %9, 1, !dbg !27
  br i1 %10, label %11, label %13, !dbg !26

11:                                               ; preds = %8
  %12 = call i32 (ptr, ...) @printf(ptr noundef @.str.1), !dbg !28
  store i32 1, ptr %2, align 4, !dbg !29
  br label %27, !dbg !29

13:                                               ; preds = %8
  %14 = load i32, ptr %3, align 4, !dbg !30
  %15 = load i32, ptr %3, align 4, !dbg !31
  %16 = sub nsw i32 %15, 1, !dbg !32
  %17 = load i32, ptr %3, align 4, !dbg !33
  %18 = sub nsw i32 %17, 2, !dbg !34
  %19 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, i32 noundef %14, i32 noundef %16, i32 noundef %18), !dbg !35
  %20 = load i32, ptr %3, align 4, !dbg !36
  %21 = sub nsw i32 %20, 1, !dbg !37
  %22 = call i32 @Fibonacci(i32 noundef %21), !dbg !38
  %23 = load i32, ptr %3, align 4, !dbg !39
  %24 = sub nsw i32 %23, 2, !dbg !40
  %25 = call i32 @Fibonacci(i32 noundef %24), !dbg !41
  %26 = add nsw i32 %22, %25, !dbg !42
  store i32 %26, ptr %2, align 4, !dbg !43
  br label %27, !dbg !43

27:                                               ; preds = %13, %11, %6
  %28 = load i32, ptr %2, align 4, !dbg !44
  ret i32 %28, !dbg !44
}

attributes #0 = { noinline nounwind optnone sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind willreturn }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7}
!llvm.ident = !{!8}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 15.0.7", isOptimized: false, runtimeVersion: 0, emissionKind: NoDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "Fibonacci.c", directory: "/home/buchy/Desktop/Uni/III/Linguaggi e Compilatori/Middle_e_back_ends-Marongiu/Linguaggi-e-Compilatori-2022-2023/Tutorial-01/TestPass/test")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = !{i32 1, !"wchar_size", i32 4}
!4 = !{i32 7, !"PIC Level", i32 2}
!5 = !{i32 7, !"PIE Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 2}
!7 = !{i32 7, !"frame-pointer", i32 2}
!8 = !{!"clang version 15.0.7"}
!9 = distinct !DISubprogram(name: "printf", scope: !1, file: !1, line: 4, type: !10, scopeLine: 4, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !11)
!10 = !DISubroutineType(types: !11)
!11 = !{}
!12 = !DILocation(line: 7, column: 3, scope: !9)
!13 = !DILocation(line: 8, column: 18, scope: !9)
!14 = !DILocation(line: 8, column: 26, scope: !9)
!15 = !DILocation(line: 8, column: 34, scope: !9)
!16 = !DILocation(line: 8, column: 9, scope: !9)
!17 = !DILocation(line: 8, column: 7, scope: !9)
!18 = !DILocation(line: 9, column: 3, scope: !9)
!19 = !DILocation(line: 11, column: 10, scope: !9)
!20 = !DILocation(line: 11, column: 3, scope: !9)
!21 = distinct !DISubprogram(name: "Fibonacci", scope: !1, file: !1, line: 14, type: !10, scopeLine: 14, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !11)
!22 = !DILocation(line: 15, column: 7, scope: !21)
!23 = !DILocation(line: 15, column: 9, scope: !21)
!24 = !DILocation(line: 16, column: 5, scope: !21)
!25 = !DILocation(line: 17, column: 5, scope: !21)
!26 = !DILocation(line: 19, column: 7, scope: !21)
!27 = !DILocation(line: 19, column: 9, scope: !21)
!28 = !DILocation(line: 20, column: 5, scope: !21)
!29 = !DILocation(line: 21, column: 5, scope: !21)
!30 = !DILocation(line: 23, column: 35, scope: !21)
!31 = !DILocation(line: 23, column: 38, scope: !21)
!32 = !DILocation(line: 23, column: 40, scope: !21)
!33 = !DILocation(line: 23, column: 45, scope: !21)
!34 = !DILocation(line: 23, column: 47, scope: !21)
!35 = !DILocation(line: 23, column: 3, scope: !21)
!36 = !DILocation(line: 24, column: 20, scope: !21)
!37 = !DILocation(line: 24, column: 22, scope: !21)
!38 = !DILocation(line: 24, column: 10, scope: !21)
!39 = !DILocation(line: 24, column: 39, scope: !21)
!40 = !DILocation(line: 24, column: 41, scope: !21)
!41 = !DILocation(line: 24, column: 29, scope: !21)
!42 = !DILocation(line: 24, column: 27, scope: !21)
!43 = !DILocation(line: 24, column: 3, scope: !21)
!44 = !DILocation(line: 25, column: 1, scope: !21)
