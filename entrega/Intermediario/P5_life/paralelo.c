/* P5 - Jogo da Vida de Conway (versao paralela com OpenMP)
 *
 * Dependencia: a geracao t+1 so le a geracao t. Dentro de uma geracao,
 * cada celula de "nxt" e escrita por uma unica iteracao e "cur" so e lido,
 * entao as celulas sao independentes. Entre geracoes ha dependencia: a
 * geracao t+1 so pode comecar quando a t terminou inteira.
 *
 * Estrategia:
 *  - Uma unica regiao paralela em volta de TODAS as geracoes (as threads
 *    sao criadas uma vez, nao G vezes).
 *  - Em cada geracao, "omp for" divide as colunas i entre as threads.
 *    A barreira implicita no fim do "omp for" e exatamente a dependencia
 *    entre geracoes: ninguem troca os tabuleiros antes de todos terminarem.
 *  - A troca cur/nxt e feita em variaveis locais de cada thread (todas
 *    fazem a mesma troca), entao nao precisa de "single" nem de outra
 *    barreira.
 *  - schedule(static): toda celula custa o mesmo, blocos contiguos de
 *    colunas por thread (cada thread percorre a memoria em sequencia).
 *  - Nao ha variavel compartilhada escrita por mais de uma thread: sem
 *    reduction, atomic ou critical.
 *
 * A leitura tem a mesma correcao do sequencial (linha curta = celula
 * morta). O tempo medido e so o das G geracoes (stderr).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

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

int main(void) {
    int n, g;
    if (scanf("%d %d", &n, &g) != 2) return 1;
    cell_t **cur = alloc_board(n), **nxt = alloc_board(n);
    char *line = malloc(n + 16);
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
    for (int j = 0; j < n; j++) {
        if (!fgets(line, n + 16, stdin)) return 1;
        size_t len = strcspn(line, "\r\n");
        for (int i = 0; i < n; i++)
            cur[i][j] = ((size_t)i < len && line[i] == 'x');
    }

    double t0 = omp_get_wtime();
    #pragma omp parallel
    {
        cell_t **c = cur, **p = nxt;          /* copias locais dos ponteiros */
        for (int s = 0; s < g; s++) {
            #pragma omp for schedule(static)
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++) {
                    int a = adjacent_to(c, n, i, j);
                    if (a == 2) p[i][j] = c[i][j];
                    else if (a == 3) p[i][j] = 1;
                    else p[i][j] = 0;
                }
            /* barreira implicita aqui: a geracao s terminou inteira */
            cell_t **t = p; p = c; c = t;
        }
    }
    double t1 = omp_get_wtime();

    /* apos G trocas, o tabuleiro final e cur se G for par, nxt se impar */
    cell_t **fim = (g % 2 == 0) ? cur : nxt;
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) putchar(fim[i][j] ? 'x' : ' ');
        putchar('\n');
    }
    fprintf(stderr, "tempo: %.6f s (threads=%d)\n", t1 - t0, omp_get_max_threads());
    free_board(cur, n); free_board(nxt, n); free(line);
    return 0;
}
