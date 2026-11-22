#include <stdio.h>
#include <stdlib.h>
typedef unsigned char cell_t;

static cell_t **alloc_board(int n) {
cell_t **b = malloc(sizeof(cell_t*) * n);
for (int i = 0; i < n; i++) b[i] = malloc(sizeof(cell_t) * n);
return b;
}
static void free_board(cell_t **b, int n) {
for (int i = 0; i < n; i++) free(b[i]);
free(b);
}
static int adjacent_to(cell_t **b, int n, int i, int j) {
int sk = (i > 0) ? i-1 : i, ek = (i+1 < n) ? i+1 : i;
int sl = (j > 0) ? j-1 : j, el = (j+1 < n) ? j+1 : j;
int count = 0;
for (int k = sk; k <= ek; k++)
for (int l = sl; l <= el; l++) count += b[k][l];
return count - b[i][j];
}
static void play(cell_t **cur, cell_t **nxt, int n) {
for (int i = 0; i < n; i++)
for (int j = 0; j < n; j++) {
int a = adjacent_to(cur, n, i, j);
if (a == 2) nxt[i][j] = cur[i][j];
else if (a == 3) nxt[i][j] = 1;
else nxt[i][j] = 0;
}
}
int main(void) {
int n, g;
if (scanf("%d %d", &n, &g) != 2) return 1;
cell_t **cur = alloc_board(n), **nxt = alloc_board(n);
char *line = malloc(n + 16);
int ch;
while ((ch = getchar()) != '\n' && ch != EOF) {}
for (int j = 0; j < n; j++) {
if (!fgets(line, n + 16, stdin)) return 1;
for (int i = 0; i < n; i++)
cur[i][j] = (line[i] == 'x');
}
for (int s = 0; s < g; s++) {
play(cur, nxt, n);
cell_t **t = nxt; nxt = cur; cur = t;
}
for (int j = 0; j < n; j++) {
for (int i = 0; i < n; i++) putchar(cur[i][j] ? 'x' : ' ');
putchar('\n');
}
free_board(cur, n); free_board(nxt, n); free(line);
return 0;
}
