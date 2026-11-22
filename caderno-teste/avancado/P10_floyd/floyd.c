#include <stdio.h>
#include <stdlib.h>
int main(void) {
int V;
if (scanf("%d", &V) != 1) return 1;
int *D = malloc(sizeof(int) * (long)V * V);
if (!D) return 1;
for (long i = 0; i < (long)V*V; i++) if (scanf("%d", &D[i]) != 1) return 1;
for (int k = 0; k < V; k++)
for (int i = 0; i < V; i++)
for (int j = 0; j < V; j++) {
int dik = D[(long)i*V+k], dkj = D[(long)k*V+j];
if (dik != -1 && dkj != -1 &&
(D[(long)i*V+j] == -1 || dik + dkj < D[(long)i*V+j]))
D[(long)i*V+j] = dik + dkj;
}
for (long i = 0; i < (long)V*V; i++) printf("%d\n", D[i]);
free(D);
return 0;
}
