// Equivalent C code to test/Loop.c as optimized with clang's -O2

int g;

int g_incr(int c) {
  g += c;
  return g;
}

int loop(int a, int b, int c) {
    // g is global: if i were to drop ans and just use g instead, LLVM would compile all references to loads and stores - maybe because it could be modified by other threads?
    int ans = g;
    if (b > a) {
        ans += (b - a) * c;
        g = ans;
    }
    return ans;
}