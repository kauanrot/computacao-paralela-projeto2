#include <stdio.h>
#include <stdlib.h>
static void merge(int *a, int *tmp, long l, long m, long r) {
long i = l, j = m+1, k = l;
while (i <= m && j <= r) tmp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
while (i <= m) tmp[k++] = a[i++];
while (j <= r) tmp[k++] = a[j++];
for (k = l; k <= r; k++) a[k] = tmp[k];
}
static void msort(int *a, int *tmp, long l, long r) {
if (l >= r) return;
long m = l + (r - l) / 2;
msort(a, tmp, l, m);
msort(a, tmp, m+1, r);
merge(a, tmp, l, m, r);
}
int main(void) {
long N;
if (scanf("%ld", &N) != 1) return 1;
int *a = malloc(sizeof(int) * N);
int *tmp = malloc(sizeof(int) * N);
if (!a || !tmp) return 1;
for (long i = 0; i < N; i++) if (scanf("%d", &a[i]) != 1) return 1;
msort(a, tmp, 0, N-1);
for (long i = 0; i < N; i++) printf("%d\n", a[i]);
free(a); free(tmp);
return 0;
}
