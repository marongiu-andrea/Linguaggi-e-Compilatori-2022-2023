; ModuleID = 'Fibonacci.c'
source_filename = "Fibonacci.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

%struct.__va_list_tag = type { i32, i32, ptr, ptr }

@stdout = external dso_local local_unnamed_addr global ptr, align 8
@.str = private unnamed_addr constant [9 x i8] c"f(0) = 0\00", align 1
@.str.1 = private unnamed_addr constant [9 x i8] c"f(1) = 1\00", align 1
@.str.2 = private unnamed_addr constant [22 x i8] c"f(%d) = f(%d) + f(%d)\00", align 1

; Function Attrs: nofree nounwind uwtable
define dso_local i32 @printf(ptr nocapture noundef readonly %0, ...) local_unnamed_addr #0 !dbg !6 {
  %2 = alloca [1 x %struct.__va_list_tag], align 16
  call void @llvm.lifetime.start.p0(i64 24, ptr nonnull %2) #4, !dbg !9
  call void @llvm.va_start(ptr nonnull %2), !dbg !10
  %3 = load ptr, ptr @stdout, align 8, !dbg !11, !tbaa !12
  %4 = call i32 @vfprintf(ptr noundef %3, ptr noundef %0, ptr noundef nonnull %2), !dbg !16
  call void @llvm.va_end(ptr nonnull %2), !dbg !17
  call void @llvm.lifetime.end.p0(i64 24, ptr nonnull %2) #4, !dbg !18
  ret i32 %4, !dbg !19
}

; Function Attrs: argmemonly mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.va_start(ptr) #2

; Function Attrs: nofree nounwind
declare dso_local noundef i32 @vfprintf(ptr nocapture noundef, ptr nocapture noundef readonly, ptr noundef) local_unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.va_end(ptr) #2

; Function Attrs: argmemonly mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: nofree nounwind uwtable
define dso_local i32 @Fibonacci(i32 noundef %0) local_unnamed_addr #0 !dbg !20 {
  br label %2, !dbg !21

2:                                                ; preds = %5, %1
  %3 = phi i32 [ 0, %1 ], [ %10, %5 ]
  %4 = phi i32 [ %0, %1 ], [ %7, %5 ]
  switch i32 %4, label %5 [
    i32 0, label %12
    i32 1, label %11
  ], !dbg !22

5:                                                ; preds = %2
  %6 = add nsw i32 %4, -1, !dbg !23
  %7 = add nsw i32 %4, -2, !dbg !24
  %8 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull @.str.2, i32 noundef %4, i32 noundef %6, i32 noundef %7), !dbg !25
  %9 = tail call i32 @Fibonacci(i32 noundef %6), !dbg !26
  %10 = add nsw i32 %9, %3, !dbg !27
  br label %2, !dbg !21

11:                                               ; preds = %2
  br label %12, !dbg !28

12:                                               ; preds = %2, %11
  %13 = phi ptr [ @.str.1, %11 ], [ @.str, %2 ]
  %14 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull %13), !dbg !28
  %15 = add nsw i32 %4, %3, !dbg !27
  ret i32 %15, !dbg !29
}

attributes #0 = { nofree nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { argmemonly mustprogress nocallback nofree nosync nounwind willreturn }
attributes #2 = { mustprogress nocallback nofree nosync nounwind willreturn }
attributes #3 = { nofree nounwind "frame-pointer"="none" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4}
!llvm.ident = !{!5}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 15.0.7 (Fedora 15.0.7-1.fc37)", isOptimized: true, runtimeVersion: 0, emissionKind: NoDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "Fibonacci.c", directory: "/home/simone/Desktop/Linguaggi-e-Compilatori-2022-2023/Tutorial-01/TestPass/test")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = !{i32 1, !"wchar_size", i32 4}
!4 = !{i32 7, !"uwtable", i32 2}
!5 = !{!"clang version 15.0.7 (Fedora 15.0.7-1.fc37)"}
!6 = distinct !DISubprogram(name: "printf", scope: !1, file: !1, line: 4, type: !7, scopeLine: 4, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !8)
!7 = !DISubroutineType(types: !8)
!8 = !{}
!9 = !DILocation(line: 6, column: 3, scope: !6)
!10 = !DILocation(line: 7, column: 3, scope: !6)
!11 = !DILocation(line: 8, column: 18, scope: !6)
!12 = !{!13, !13, i64 0}
!13 = !{!"any pointer", !14, i64 0}
!14 = !{!"omnipotent char", !15, i64 0}
!15 = !{!"Simple C/C++ TBAA"}
!16 = !DILocation(line: 8, column: 9, scope: !6)
!17 = !DILocation(line: 9, column: 3, scope: !6)
!18 = !DILocation(line: 12, column: 1, scope: !6)
!19 = !DILocation(line: 11, column: 3, scope: !6)
!20 = distinct !DISubprogram(name: "Fibonacci", scope: !1, file: !1, line: 14, type: !7, scopeLine: 14, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !8)
!21 = !DILocation(line: 24, column: 29, scope: !20)
!22 = !DILocation(line: 15, column: 7, scope: !20)
!23 = !DILocation(line: 23, column: 40, scope: !20)
!24 = !DILocation(line: 23, column: 47, scope: !20)
!25 = !DILocation(line: 23, column: 3, scope: !20)
!26 = !DILocation(line: 24, column: 10, scope: !20)
!27 = !DILocation(line: 24, column: 27, scope: !20)
!28 = !DILocation(line: 0, scope: !20)
!29 = !DILocation(line: 25, column: 1, scope: !20)
