//int N=100;
#define N 100
void populate (int a[N] , int b[N], int c[N]) {
  int i;
  for (i=0; i<N; i++) a[i]=5*c[i];
  // aggiungere una printf per rompere un controllo di adiacenza
  // fatto male 
  //printf();
  for (i=0; i<N; i++) b[i] = a[i]+c[i];
}