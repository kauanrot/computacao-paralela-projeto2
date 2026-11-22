#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INT_TYPE unsigned long long
#define INT_TYPE_SIZE (sizeof(INT_TYPE) * 8)
#define CELL_VAL_SIZE 1
#define MAX_BDIM 8

typedef struct { INT_TYPE v[CELL_VAL_SIZE]; } cell_v;
typedef struct { int r, c; } cell_coord;

typedef struct sudoku {
int bdim, dim, peers_size;
int *grid;
cell_coord ****unit_list;
cell_coord ***peers;
cell_v **values;
unsigned long long sol_count;
} sudoku;

static inline int cell_v_get(cell_v *v, int p) {
return !!((*v).v[(p-1)/INT_TYPE_SIZE] & (((INT_TYPE)1) << ((p-1)%INT_TYPE_SIZE)));
}
static inline void cell_v_unset(cell_v *v, int p) {
(*v).v[(p-1)/INT_TYPE_SIZE] &= ~(((INT_TYPE)1) << ((p-1)%INT_TYPE_SIZE));
}
static inline void cell_v_set(cell_v *v, int p) {
(*v).v[(p-1)/INT_TYPE_SIZE] |= ((INT_TYPE)1) << ((p-1)%INT_TYPE_SIZE);
}
static inline int cell_v_count(cell_v *v) {
int acc = 0;
for (int i = 0; i < CELL_VAL_SIZE; i++) acc += __builtin_popcountll((*v).v[i]);
return acc;
}
static inline int digit_get(cell_v *v) {
if (cell_v_count(v) != 1) return -1;
for (int i = 0; i < CELL_VAL_SIZE; i++)
if ((*v).v[i]) return 1 + INT_TYPE_SIZE*i + __builtin_ctzll((*v).v[i]);
return -1;
}

static int assign(sudoku *s, int i, int j, int d);

static int eliminate(sudoku *s, int i, int j, int d) {
if (!cell_v_get(&s->values[i][j], d)) return 1;
cell_v_unset(&s->values[i][j], d);
int cnt = cell_v_count(&s->values[i][j]);
if (cnt == 0) return 0;
if (cnt == 1)
for (int k = 0; k < s->peers_size; k++)
if (!eliminate(s, s->peers[i][j][k].r, s->peers[i][j][k].c,
digit_get(&s->values[i][j]))) return 0;
for (int k = 0; k < 3; k++) {
int cont = 0, pos = 0;
cell_coord *u = s->unit_list[i][j][k];
for (int ii = 0; ii < s->dim; ii++)
if (cell_v_get(&s->values[u[ii].r][u[ii].c], d)) { cont++; pos = ii; }
if (cont == 0) return 0;
if (cont == 1 && !assign(s, u[pos].r, u[pos].c, d)) return 0;
}
return 1;
}

static int assign(sudoku *s, int i, int j, int d) {
for (int d2 = 1; d2 <= s->dim; d2++)
if (d2 != d && !eliminate(s, i, j, d2)) return 0;
return 1;
}

#ifdef DEBUG
static void display(sudoku *s) {
printf("%d\n", s->bdim);
for (int i = 0; i < s->dim; i++) {
for (int j = 0; j < s->dim; j++)
printf("%d ", digit_get(&s->values[i][j]));
printf("\n");
}
}
#endif

static void destroy_sudoku(sudoku *s) {
for (int i = 0; i < s->dim; i++) {
for (int j = 0; j < s->dim; j++) {
for (int k = 0; k < 3; k++) free(s->unit_list[i][j][k]);
free(s->unit_list[i][j]);
}
free(s->unit_list[i]);
}
free(s->unit_list);
for (int i = 0; i < s->dim; i++) {
for (int j = 0; j < s->dim; j++) free(s->peers[i][j]);
free(s->peers[i]);
}
free(s->peers);
for (int i = 0; i < s->dim; i++) free(s->values[i]);
free(s->values);
free(s);
}

static void init(sudoku *s) {
int i, j, k, l, pos;
for (i = 0; i < s->dim; i++) {
int ibase = i / s->bdim * s->bdim;
for (j = 0; j < s->dim; j++) {
for (pos = 0; pos < s->dim; pos++) {
s->unit_list[i][j][0][pos].r = i;
s->unit_list[i][j][0][pos].c = pos;
s->unit_list[i][j][1][pos].r = pos;
s->unit_list[i][j][1][pos].c = j;
}
int jbase = j / s->bdim * s->bdim;
for (pos = 0, k = 0; k < s->bdim; k++)
for (l = 0; l < s->bdim; l++, pos++) {
s->unit_list[i][j][2][pos].r = ibase + k;
s->unit_list[i][j][2][pos].c = jbase + l;
}
}
}
for (i = 0; i < s->dim; i++)
for (j = 0; j < s->dim; j++) {
pos = 0;
for (k = 0; k < s->dim; k++)
if (s->unit_list[i][j][0][k].c != j)
s->peers[i][j][pos++] = s->unit_list[i][j][0][k];
for (k = 0; k < s->dim; k++) {
cell_coord sq = s->unit_list[i][j][1][k];
if (sq.r != i) s->peers[i][j][pos++] = sq;
sq = s->unit_list[i][j][2][k];
if (sq.r != i && sq.c != j) s->peers[i][j][pos++] = sq;
}
}
}

static int parse_grid(sudoku *s) {
int i, j, k;
int *ld = s->grid;
for (i = 0; i < s->dim; i++)
for (j = 0; j < s->dim; j++)
for (k = 1; k <= s->dim; k++)
cell_v_set(&s->values[i][j], k);
for (i = 0; i < s->dim; i++)
for (j = 0; j < s->dim; j++)
if (ld[i*s->dim+j] > 0 && !assign(s, i, j, ld[i*s->dim+j]))
return 0;
return 1;
}

static sudoku *create_sudoku(int bdim, int *grid) {
if (bdim > MAX_BDIM) return NULL;
sudoku *r = calloc(1, sizeof(sudoku));
if (!r) return NULL;
r->bdim = bdim;
int dim = bdim * bdim;
r->dim = dim;
r->peers_size = 3 * dim - 2 * bdim - 1;
r->grid = grid;
r->sol_count = 0;
r->unit_list = malloc(sizeof(cell_coord***) * dim);
for (int i = 0; i < dim; i++) {
r->unit_list[i] = malloc(sizeof(cell_coord**) * dim);
for (int j = 0; j < dim; j++) {
r->unit_list[i][j] = malloc(sizeof(cell_coord*) * 3);
for (int k = 0; k < 3; k++)
r->unit_list[i][j][k] = calloc(dim, sizeof(cell_coord));
}
}
r->peers = malloc(sizeof(cell_coord**) * dim);
for (int i = 0; i < dim; i++) {
r->peers[i] = malloc(sizeof(cell_coord*) * dim);
for (int j = 0; j < dim; j++)
r->peers[i][j] = calloc(r->peers_size, sizeof(cell_coord));
}
r->values = malloc(sizeof(cell_v*) * dim);
for (int i = 0; i < dim; i++) r->values[i] = calloc(dim, sizeof(cell_v));
init(r);
if (!parse_grid(r)) { destroy_sudoku(r); return NULL; }
return r;
}

static void search(sudoku *s) {
int solved = 1;
for (int i = 0; solved && i < s->dim; i++)
for (int j = 0; j < s->dim; j++)
if (cell_v_count(&s->values[i][j]) != 1) { solved = 0; break; }
if (solved) { s->sol_count++; return; }

int min = INT_MAX, minI = -1, minJ = -1;
for (int i = 0; i < s->dim; i++)
for (int j = 0; j < s->dim; j++) {
int used = cell_v_count(&s->values[i][j]);
if (used > 1 && used < min) { min = used; minI = i; minJ = j; }
}
if (minI < 0) return;

cell_v **bk = malloc(sizeof(cell_v*) * s->dim);
for (int i = 0; i < s->dim; i++) bk[i] = malloc(sizeof(cell_v) * s->dim);

for (int k = 1; k <= s->dim; k++) {
if (!cell_v_get(&s->values[minI][minJ], k)) continue;
for (int i = 0; i < s->dim; i++)
for (int j = 0; j < s->dim; j++) bk[i][j] = s->values[i][j];
if (assign(s, minI, minJ, k)) search(s);
for (int i = 0; i < s->dim; i++)
for (int j = 0; j < s->dim; j++) s->values[i][j] = bk[i][j];
}
for (int i = 0; i < s->dim; i++) free(bk[i]);
free(bk);
}

int main(void) {
int b;
if (scanf("%d", &b) != 1 || b > MAX_BDIM) {
fprintf(stderr, "erro: bdim invalido\n"); return 1;
}
int dim = b*b, total = dim*dim;
int *grid = malloc(sizeof(int)*total);
for (int i = 0; i < total; i++)
if (scanf("%d", &grid[i]) != 1) {
fprintf(stderr, "erro: leitura incompleta em %d\n", i);
free(grid); return 1;
}
sudoku *s = create_sudoku(b, grid);
if (!s) { fprintf(stderr, "erro: tabuleiro invalido\n"); free(grid); return 1; }
search(s);
printf("%llu\n", s->sol_count);
destroy_sudoku(s);
free(grid);
return 0;
}
