; ModuleID = 'Loop.c'
source_filename = "Loop.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@g = dso_local local_unnamed_addr global i32 0, align 4

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn uwtable
define dso_local i32 @g_incr(i32 noundef %0) local_unnamed_addr #0 !dbg !6 {
  %2 = load i32, ptr @g, align 4, !dbg !9, !tbaa !10
  %3 = add nsw i32 %2, %0, !dbg !9
  store i32 %3, ptr @g, align 4, !dbg !9, !tbaa !10
  ret i32 %3, !dbg !14
}

; Function Attrs: nofree norecurse nosync nounwind uwtable
define dso_local i32 @loop(i32 noundef %0, i32 noundef %1, i32 noundef %2) local_unnamed_addr #1 !dbg !15 {
  %4 = load i32, ptr @g, align 4, !tbaa !10
  %5 = icmp sgt i32 %1, %0, !dbg !16
  br i1 %5, label %6, label %10, !dbg !17

6:                                                ; preds = %3
  %7 = sub i32 %1, %0, !dbg !17
  %8 = mul i32 %7, %2, !dbg !17
  %9 = add i32 %4, %8, !dbg !17
  store i32 %9, ptr @g, align 4, !dbg !18, !tbaa !10
  br label %10, !dbg !17

10:                                               ; preds = %6, %3
  %11 = phi i32 [ %9, %6 ], [ %4, %3 ], !dbg !20
  ret i32 %11, !dbg !21
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree norecurse nosync nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4}
!llvm.ident = !{!5}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 15.0.7 (Fedora 15.0.7-1.fc37)", isOptimized: true, runtimeVersion: 0, emissionKind: NoDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "Loop.c", directory: "/home/simone/Desktop/Linguaggi-e-Compilatori-2022-2023/Tutorial-01/TestPass/test")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = !{i32 1, !"wchar_size", i32 4}
!4 = !{i32 7, !"uwtable", i32 2}
!5 = !{!"clang version 15.0.7 (Fedora 15.0.7-1.fc37)"}
!6 = distinct !DISubprogram(name: "g_incr", scope: !1, file: !1, line: 20, type: !7, scopeLine: 20, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !8)
!7 = !DISubroutineType(types: !8)
!8 = !{}
!9 = !DILocation(line: 21, column: 5, scope: !6)
!10 = !{!11, !11, i64 0}
!11 = !{!"int", !12, i64 0}
!12 = !{!"omnipotent char", !13, i64 0}
!13 = !{!"Simple C/C++ TBAA"}
!14 = !DILocation(line: 22, column: 3, scope: !6)
!15 = distinct !DISubprogram(name: "loop", scope: !1, file: !1, line: 30, type: !7, scopeLine: 30, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !8)
!16 = !DILocation(line: 33, column: 17, scope: !15)
!17 = !DILocation(line: 33, column: 3, scope: !15)
!18 = !DILocation(line: 21, column: 5, scope: !6, inlinedAt: !19)
!19 = distinct !DILocation(line: 34, column: 5, scope: !15)
!20 = !DILocation(line: 37, column: 16, scope: !15)
!21 = !DILocation(line: 37, column: 3, scope: !15)
