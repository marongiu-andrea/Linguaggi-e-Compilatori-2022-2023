void populate (int* a, int* b, int* c, int N) {
  int i;

  for(i=0; i<N; i++) {
    a[i] = 5*c[i];
  }

  for(i=0; i<N; i++) {
    b[i] = a[i]+c[i];
  }
}