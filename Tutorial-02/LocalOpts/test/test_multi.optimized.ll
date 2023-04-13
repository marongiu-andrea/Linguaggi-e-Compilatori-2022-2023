; ModuleID = 'test/test_Multi.optimized.bc'
source_filename = "test/test_multi.ll"

define dso_local void @foo(i32 noundef %0) {
  %2 = add nsw i32 %0, 5
  ret void
}
