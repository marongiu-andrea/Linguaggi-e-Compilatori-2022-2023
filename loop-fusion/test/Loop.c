void populate (int* a, int* b, int* c, int N) {
  for(int i = 0; i < N; i++)
  {
    a[i] = 5 * c[i];
  }

  for(int i = 0; i < N; i++)
  {
    b[i] = a[i] + c[i];
  }
}