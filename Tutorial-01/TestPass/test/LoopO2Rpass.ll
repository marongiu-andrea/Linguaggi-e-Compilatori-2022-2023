; ModuleID = 'test/Loop.c'
source_filename = "test/Loop.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@g = dso_local local_unnamed_addr global i32 0, align 4

; Function Attrs: mustprogress nofree norecurse nosync nounwind uwtable willreturn
define dso_local i32 @g_incr(i32 noundef %0) local_unnamed_addr #0 !dbg !8 {
  %2 = load i32, i32* @g, align 4, !dbg !11, !tbaa !12
  %3 = add nsw i32 %2, %0, !dbg !11
  store i32 %3, i32* @g, align 4, !dbg !11, !tbaa !12
  ret i32 %3, !dbg !16
}

; Function Attrs: nofree norecurse nosync nounwind uwtable
define dso_local i32 @loop(i32 noundef %0, i32 noundef %1, i32 noundef %2) local_unnamed_addr #1 !dbg !17 {
  %4 = load i32, i32* @g, align 4, !tbaa !12
  %5 = icmp sgt i32 %1, %0, !dbg !18
  br i1 %5, label %6, label %10, !dbg !19

6:                                                ; preds = %3
  %7 = sub i32 %1, %0, !dbg !19
  %8 = mul i32 %7, %2, !dbg !19
  %9 = add i32 %4, %8, !dbg !19
  store i32 %9, i32* @g, align 4, !dbg !20, !tbaa !12
  br label %10, !dbg !19

10:                                               ; preds = %6, %3
  %11 = phi i32 [ %9, %6 ], [ %4, %3 ], !dbg !22
  ret i32 %11, !dbg !23
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind uwtable willreturn "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree norecurse nosync nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "Debian clang version 14.0.6", isOptimized: true, runtimeVersion: 0, emissionKind: NoDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "test/Loop.c", directory: "/mnt/c/C++/Linguaggi-e-Compilatori-2022-2023/Tutorial-01/TestPass")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = !{i32 1, !"wchar_size", i32 4}
!4 = !{i32 7, !"PIC Level", i32 2}
!5 = !{i32 7, !"PIE Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 1}
!7 = !{!"Debian clang version 14.0.6"}
!8 = distinct !DISubprogram(name: "g_incr", scope: !1, file: !1, line: 20, type: !9, scopeLine: 20, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !10)
!9 = !DISubroutineType(types: !10)
!10 = !{}
!11 = !DILocation(line: 21, column: 5, scope: !8)
!12 = !{!13, !13, i64 0}
!13 = !{!"int", !14, i64 0}
!14 = !{!"omnipotent char", !15, i64 0}
!15 = !{!"Simple C/C++ TBAA"}
!16 = !DILocation(line: 22, column: 3, scope: !8)
!17 = distinct !DISubprogram(name: "loop", scope: !1, file: !1, line: 30, type: !9, scopeLine: 30, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !10)
!18 = !DILocation(line: 33, column: 17, scope: !17)
!19 = !DILocation(line: 33, column: 3, scope: !17)
!20 = !DILocation(line: 21, column: 5, scope: !8, inlinedAt: !21)
!21 = distinct !DILocation(line: 34, column: 5, scope: !17)
!22 = !DILocation(line: 37, column: 16, scope: !17)
!23 = !DILocation(line: 37, column: 3, scope: !17)
