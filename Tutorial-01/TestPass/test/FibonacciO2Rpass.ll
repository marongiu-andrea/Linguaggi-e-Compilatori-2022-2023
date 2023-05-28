; ModuleID = 'test/Fibonacci.c'
source_filename = "test/Fibonacci.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %struct._IO_marker*, %struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, %struct._IO_codecvt*, %struct._IO_wide_data*, %struct._IO_FILE*, i8*, i64, i32, [20 x i8] }
%struct._IO_marker = type opaque
%struct._IO_codecvt = type opaque
%struct._IO_wide_data = type opaque
%struct.__va_list_tag = type { i32, i32, i8*, i8* }

@stdout = external local_unnamed_addr global %struct._IO_FILE*, align 8
@.str = private unnamed_addr constant [9 x i8] c"f(0) = 0\00", align 1
@.str.1 = private unnamed_addr constant [9 x i8] c"f(1) = 1\00", align 1
@.str.2 = private unnamed_addr constant [22 x i8] c"f(%d) = f(%d) + f(%d)\00", align 1

; Function Attrs: nofree nounwind uwtable
define dso_local i32 @printf(i8* nocapture noundef readonly %0, ...) local_unnamed_addr #0 !dbg !8 {
  %2 = alloca [1 x %struct.__va_list_tag], align 16
  %3 = bitcast [1 x %struct.__va_list_tag]* %2 to i8*, !dbg !11
  call void @llvm.lifetime.start.p0i8(i64 24, i8* nonnull %3) #4, !dbg !11
  %4 = getelementptr inbounds [1 x %struct.__va_list_tag], [1 x %struct.__va_list_tag]* %2, i64 0, i64 0, !dbg !12
  call void @llvm.va_start(i8* nonnull %3), !dbg !12
  %5 = load %struct._IO_FILE*, %struct._IO_FILE** @stdout, align 8, !dbg !13, !tbaa !14
  %6 = call i32 @vfprintf(%struct._IO_FILE* noundef %5, i8* noundef %0, %struct.__va_list_tag* noundef nonnull %4), !dbg !18
  call void @llvm.va_end(i8* nonnull %3), !dbg !19
  call void @llvm.lifetime.end.p0i8(i64 24, i8* nonnull %3) #4, !dbg !20
  ret i32 %6, !dbg !21
}

; Function Attrs: argmemonly mustprogress nofree nosync nounwind willreturn
declare void @llvm.lifetime.start.p0i8(i64 immarg, i8* nocapture) #1

; Function Attrs: mustprogress nofree nosync nounwind willreturn
declare void @llvm.va_start(i8*) #2

; Function Attrs: nofree nounwind
declare noundef i32 @vfprintf(%struct._IO_FILE* nocapture noundef, i8* nocapture noundef readonly, %struct.__va_list_tag* noundef) local_unnamed_addr #3

; Function Attrs: mustprogress nofree nosync nounwind willreturn
declare void @llvm.va_end(i8*) #2

; Function Attrs: argmemonly mustprogress nofree nosync nounwind willreturn
declare void @llvm.lifetime.end.p0i8(i64 immarg, i8* nocapture) #1

; Function Attrs: nofree nounwind uwtable
define dso_local i32 @Fibonacci(i32 noundef %0) local_unnamed_addr #0 !dbg !22 {
  br label %2, !dbg !23

2:                                                ; preds = %5, %1
  %3 = phi i32 [ 0, %1 ], [ %10, %5 ]
  %4 = phi i32 [ %0, %1 ], [ %7, %5 ]
  switch i32 %4, label %5 [
    i32 0, label %12
    i32 1, label %11
  ], !dbg !24

5:                                                ; preds = %2
  %6 = add nsw i32 %4, -1, !dbg !25
  %7 = add nsw i32 %4, -2, !dbg !26
  %8 = tail call i32 (i8*, ...) @printf(i8* noundef nonnull dereferenceable(1) getelementptr inbounds ([22 x i8], [22 x i8]* @.str.2, i64 0, i64 0), i32 noundef %4, i32 noundef %6, i32 noundef %7), !dbg !27
  %9 = tail call i32 @Fibonacci(i32 noundef %6), !dbg !28
  %10 = add nsw i32 %9, %3, !dbg !29
  br label %2, !dbg !23

11:                                               ; preds = %2
  br label %12, !dbg !30

12:                                               ; preds = %2, %11
  %13 = phi i8* [ getelementptr inbounds ([9 x i8], [9 x i8]* @.str.1, i64 0, i64 0), %11 ], [ getelementptr inbounds ([9 x i8], [9 x i8]* @.str, i64 0, i64 0), %2 ]
  %14 = tail call i32 (i8*, ...) @printf(i8* noundef nonnull dereferenceable(1) %13), !dbg !30
  %15 = add nsw i32 %4, %3, !dbg !29
  ret i32 %15, !dbg !31
}

attributes #0 = { nofree nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { argmemonly mustprogress nofree nosync nounwind willreturn }
attributes #2 = { mustprogress nofree nosync nounwind willreturn }
attributes #3 = { nofree nounwind "frame-pointer"="none" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "Debian clang version 14.0.6", isOptimized: true, runtimeVersion: 0, emissionKind: NoDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "test/Fibonacci.c", directory: "/mnt/c/C++/Linguaggi-e-Compilatori-2022-2023/Tutorial-01/TestPass")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = !{i32 1, !"wchar_size", i32 4}
!4 = !{i32 7, !"PIC Level", i32 2}
!5 = !{i32 7, !"PIE Level", i32 2}
!6 = !{i32 7, !"uwtable", i32 1}
!7 = !{!"Debian clang version 14.0.6"}
!8 = distinct !DISubprogram(name: "printf", scope: !1, file: !1, line: 4, type: !9, scopeLine: 4, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !10)
!9 = !DISubroutineType(types: !10)
!10 = !{}
!11 = !DILocation(line: 6, column: 3, scope: !8)
!12 = !DILocation(line: 7, column: 3, scope: !8)
!13 = !DILocation(line: 8, column: 18, scope: !8)
!14 = !{!15, !15, i64 0}
!15 = !{!"any pointer", !16, i64 0}
!16 = !{!"omnipotent char", !17, i64 0}
!17 = !{!"Simple C/C++ TBAA"}
!18 = !DILocation(line: 8, column: 9, scope: !8)
!19 = !DILocation(line: 9, column: 3, scope: !8)
!20 = !DILocation(line: 12, column: 1, scope: !8)
!21 = !DILocation(line: 11, column: 3, scope: !8)
!22 = distinct !DISubprogram(name: "Fibonacci", scope: !1, file: !1, line: 14, type: !9, scopeLine: 14, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !10)
!23 = !DILocation(line: 24, column: 29, scope: !22)
!24 = !DILocation(line: 15, column: 7, scope: !22)
!25 = !DILocation(line: 23, column: 40, scope: !22)
!26 = !DILocation(line: 23, column: 47, scope: !22)
!27 = !DILocation(line: 23, column: 3, scope: !22)
!28 = !DILocation(line: 24, column: 10, scope: !22)
!29 = !DILocation(line: 24, column: 27, scope: !22)
!30 = !DILocation(line: 0, scope: !22)
!31 = !DILocation(line: 25, column: 1, scope: !22)
