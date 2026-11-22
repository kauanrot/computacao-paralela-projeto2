#include <stdio.h>
int main(void) {
long M;
if (scanf("%ld", &M) != 1) return 1;
long histo[256] = {0};
for (long i = 0; i < M; i++) {
int v;
if (scanf("%d", &v) != 1) return 1;
if (v < 0 || v > 255) return 1;
histo[v]++;
}
for (int b = 0; b < 256; b++) printf("%ld\n", histo[b]);
return 0;
}
