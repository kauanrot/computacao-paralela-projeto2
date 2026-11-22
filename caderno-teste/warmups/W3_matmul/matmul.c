#include <stdio.h>
#include <stdlib.h>
int main(void) {
int N;
if (scanf("%d", &N) != 1) return 1;
long n = (long)N * N;
double *A = malloc(sizeof(double) * n);
double *B = malloc(sizeof(double) * n);
double *C = malloc(sizeof(double) * n);
if (!A || !B || !C) return 1;
for (long i = 0; i < n; i++) if (scanf("%lf", &A[i]) != 1) return 1;
for (long i = 0; i < n; i++) if (scanf("%lf", &B[i]) != 1) return 1;
for (int i = 0; i < N; i++)
for (int j = 0; j < N; j++) {
double s = 0.0;
for (int k = 0; k < N; k++)
s += A[(long)i*N+k] * B[(long)k*N+j];
C[(long)i*N+j] = s;
}
for (long i = 0; i < n; i++) printf("%.6f\n", C[i]);
free(A); free(B); free(C);
return 0;
}
