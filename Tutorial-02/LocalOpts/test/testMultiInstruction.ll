define dso_local void @foo(i32 noundef %0) {
  %2 = sub nsw i32 %0, 1
  %3 = add nsw i32 %2, 1
  %4 = add nsw i32 %3, 10
  ret void
}