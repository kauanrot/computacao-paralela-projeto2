/* P5 - Jogo da Vida de Conway (versao paralela com OpenMP)
 *
 * O codigo e igual ao sequencial; a unica mudanca e o #pragma em play().
 *
 * Por que da para paralelizar: existem dois tabuleiros. A geracao nova
 * (nxt) e calculada lendo so a geracao atual (cur). Cada celula de nxt e
 * escrita por uma unica iteracao e cur so e lido, entao as celulas de uma
 * mesma geracao sao independentes e podem ser divididas entre as threads.
 *
 * Dependencia entre geracoes: a geracao t+1 so pode comecar quando a t
 * terminou inteira. Quem garante isso e a barreira implicita no fim do
 * "parallel for": play() so retorna quando todas as threads terminaram
 * suas colunas. So depois disso os tabuleiros sao trocados (por uma
 * thread so, no main, como no sequencial).
 *
 * schedule(static): toda celula custa o mesmo, entao blocos fixos de
 * colunas por thread dividem a carga por igual.
 * Nenhuma posicao e escrita por duas threads: sem reduction/atomic/critical.
 * (As threads sao criadas uma vez e reaproveitadas pelo OpenMP nas
 * geracoes seguintes, entao repetir o "parallel for" a cada geracao
 * custa pouco.)
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

static void play(cell_t **cur, cell_t **nxt, int n) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            int a = adjacent_to(cur, n, i, j);
            if (a == 2) nxt[i][j] = cur[i][j];
            else if (a == 3) nxt[i][j] = 1;
            else nxt[i][j] = 0;
        }
    /* barreira implicita: todas as threads terminaram esta geracao */
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
        size_t len = strcspn(line, "\r\n");          /* fim real da linha */
        for (int i = 0; i < n; i++)
            cur[i][j] = ((size_t)i < len && line[i] == 'x');
    }

    double t0 = omp_get_wtime();
    for (int s = 0; s < g; s++) {
        play(cur, nxt, n);
        cell_t **t = nxt; nxt = cur; cur = t;
    }
    double t1 = omp_get_wtime();

    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) putchar(cur[i][j] ? 'x' : ' ');
        putchar('\n');
    }
    fprintf(stderr, "tempo: %.6f s (threads=%d)\n", t1 - t0, omp_get_max_threads());
    free_board(cur, n); free_board(nxt, n); free(line);
    return 0;
}
