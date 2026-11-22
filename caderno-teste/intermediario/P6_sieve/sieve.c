#include <stdio.h>
#include <stdlib.h>
int main(void) {
long N;
if (scanf("%ld", &N) != 1) return 1;
if (N < 2) { printf("0\n"); return 0; }
char *p = calloc((size_t)N + 1, 1);
if (!p) return 1;
long count = 0;
for (long i = 2; i <= N; i++)
if (!p[i]) {
count++;
for (long j = i * i; j <= N && j > 0; j += i) p[j] = 1;
}
printf("%ld\n", count);
free(p);
return 0;
}
