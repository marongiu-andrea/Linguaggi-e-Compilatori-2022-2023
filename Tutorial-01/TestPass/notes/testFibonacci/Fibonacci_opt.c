// Equivalent C code to test/Fibonacci.c optimized with clang's -O2

#include <stdarg.h>
#include <stdio.h>

int printf(const char *format, ...) {
  int ret;
  va_list args;
  va_start(args, format);
  ret = vfprintf(stdout, format, args);
  va_end(args);

  return ret;
}

/*
  No idea how else to write this and mantain the same output as the original.
  while (n >= 2) is different because the original runs indefinitely if n < 0.
  
  This has a very different output:
  while (n != 0 && n != 1) {
    printf("f(%d) = f(%d) + f(%d)", n, n - 1, n - 2);
    acc += Fibonacci(n - 1);
    n = n - 2;
  }
  if (n == 0)
    printf("f(0) = 0");
  if (n == 1)
    printf("f(1) = 1");
  
  */
int Fibonacci(int n) {
  int acc = 0;
  
  while (1>0) {
    if (n == 0) {
      printf("f(0) = 0");
      break;
    }
    if (n == 1) {
      printf("f(1) = 1");
      break;
    }

    printf("f(%d) = f(%d) + f(%d)", n, n - 1, n - 2);
    acc += Fibonacci(n - 1);
    n = n - 2;
  }

  return n + acc;
}
