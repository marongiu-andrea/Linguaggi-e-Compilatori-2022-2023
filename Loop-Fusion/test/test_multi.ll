

; void fun(int a) {
;   int r0 = a + 5;
;   int r1 = r0 - 5;
; }





define dso_local void @foo(i32 noundef %0) {
  %2 = add nsw i32 %0, 5
  %3 = sub nsw i32 %2, 5
 
  ret void
}