;void foo(void)
;{
;    int a = 10;
;    int b = a / 2;
;    int c = b * 4;
;    int d = b + 10;
;    int e = d - 10;
;}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @foo() #0 {
  %1 = add i32 0, 10
  %2 = sdiv i32 %1, 2
  %3 = mul nsw i32 %2, 4
  %4 = add i32 %2, 10
  %5 = sub i32 %4, 10
  
  ret void
}