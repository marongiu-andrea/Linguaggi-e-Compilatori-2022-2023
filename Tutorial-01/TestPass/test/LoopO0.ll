; ModuleID = 'Loop.c'
source_filename = "Loop.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@g = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local i32 @g_incr(i32 noundef %0) #0 !dbg !9 {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %3 = load i32, ptr %2, align 4, !dbg !12
  %4 = load i32, ptr @g, align 4, !dbg !13
  %5 = add nsw i32 %4, %3, !dbg !13
  store i32 %5, ptr @g, align 4, !dbg !13
  %6 = load i32, ptr @g, align 4, !dbg !14
  ret i32 %6, !dbg !15
}

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local i32 @loop(i32 noundef %0, i32 noundef %1, i32 noundef %2) #0 !dbg !16 {
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  store i32 %0, ptr %4, align 4
  store i32 %1, ptr %5, align 4
  store i32 %2, ptr %6, align 4
  store i32 0, ptr %8, align 4, !dbg !17
  %9 = load i32, ptr %4, align 4, !dbg !18
  store i32 %9, ptr %7, align 4, !dbg !19
  br label %10, !dbg !20

10:                                               ; preds = %17, %3
  %11 = load i32, ptr %7, align 4, !dbg !21
  %12 = load i32, ptr %5, align 4, !dbg !22
  %13 = icmp slt i32 %11, %12, !dbg !23
  br i1 %13, label %14, label %20, !dbg !24

14:                                               ; preds = %10
  %15 = load i32, ptr %6, align 4, !dbg !25
  %16 = call i32 @g_incr(i32 noundef %15), !dbg !26
  br label %17, !dbg !27

17:                                               ; preds = %14
  %18 = load i32, ptr %7, align 4, !dbg !28
  %19 = add nsw i32 %18, 1, !dbg !28
  store i32 %19, ptr %7, align 4, !dbg !28
  br label %10, !dbg !24, !llvm.loop !29

20:                                               ; preds = %10
  %21 = load i32, ptr %8, align 4, !dbg !31
  %22 = load i32, ptr @g, align 4, !dbg !32
  %23 = add nsw i32 %21, %22, !dbg !33
  ret i32 %23, !dbg !34
}

attributes #0 = { noinline nounwind optnone sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7}
!llvm.ident = !{!8}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 15.0.7", isOptimized: false, runtimeVersion: 0, emissionKind: NoDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "Loop.c", directory: "/home/buchy/Desktop/Uni/III/Linguaggi e Compilatori/Middle_e_back_ends-Marongiu/Linguaggi-e-Compilatori-2022-2023/Tutorial-01/TestPass/test")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = !{i32 1, !"wchar_size", i32 4}
!4 = !{i32 7, !"PIC Level", i32 2}
!5 = !{i32 7, !"PIE Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 2}
!7 = !{i32 7, !"frame-pointer", i32 2}
!8 = !{!"clang version 15.0.7"}
!9 = distinct !DISubprogram(name: "g_incr", scope: !1, file: !1, line: 20, type: !10, scopeLine: 20, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !11)
!10 = !DISubroutineType(types: !11)
!11 = !{}
!12 = !DILocation(line: 21, column: 8, scope: !9)
!13 = !DILocation(line: 21, column: 5, scope: !9)
!14 = !DILocation(line: 22, column: 10, scope: !9)
!15 = !DILocation(line: 22, column: 3, scope: !9)
!16 = distinct !DISubprogram(name: "loop", scope: !1, file: !1, line: 30, type: !10, scopeLine: 30, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !11)
!17 = !DILocation(line: 31, column: 10, scope: !16)
!18 = !DILocation(line: 33, column: 12, scope: !16)
!19 = !DILocation(line: 33, column: 10, scope: !16)
!20 = !DILocation(line: 33, column: 8, scope: !16)
!21 = !DILocation(line: 33, column: 15, scope: !16)
!22 = !DILocation(line: 33, column: 19, scope: !16)
!23 = !DILocation(line: 33, column: 17, scope: !16)
!24 = !DILocation(line: 33, column: 3, scope: !16)
!25 = !DILocation(line: 34, column: 12, scope: !16)
!26 = !DILocation(line: 34, column: 5, scope: !16)
!27 = !DILocation(line: 35, column: 3, scope: !16)
!28 = !DILocation(line: 33, column: 23, scope: !16)
!29 = distinct !{!29, !24, !27, !30}
!30 = !{!"llvm.loop.mustprogress"}
!31 = !DILocation(line: 37, column: 10, scope: !16)
!32 = !DILocation(line: 37, column: 16, scope: !16)
!33 = !DILocation(line: 37, column: 14, scope: !16)
!34 = !DILocation(line: 37, column: 3, scope: !16)
