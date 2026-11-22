/* P8 - Fractal de Mandelbrot (versao paralela com OpenMP)
 *
 * Dependencia: nenhuma entre pixels. Cada pixel so le (x, y, W, H, Imax)
 * e escreve a propria posicao de grid; nenhuma variavel compartilhada e
 * escrita por duas threads. Sem reduction, atomic ou critical.
 *
 * Problema: balanceamento de carga. Um pixel dentro do conjunto custa Imax
 * iteracoes; um pixel longe escapa em 1-2. As linhas do meio da imagem
 * custam muito mais que as das bordas. Com schedule(static) cada thread
 * recebe um bloco contiguo de linhas, e quem pega o meio trabalha muito
 * mais (as outras ficam esperando na barreira final).
 *
 * Estrategia:
 *  - parallel for nas linhas (y). Linha inteira por vez: granularidade boa
 *    (W pixels) e cada thread escreve um trecho contiguo de grid.
 *  - schedule(runtime) para poder comparar static/dynamic/guided sem
 *    recompilar (variavel OMP_SCHEDULE). Se OMP_SCHEDULE nao estiver
 *    definida, usa dynamic,1: as threads pegam a proxima linha livre
 *    assim que terminam a atual, o que equilibra a carga.
 *  - x, y, cr, ci, zr, zi, it, t sao declarados dentro do laco: privados.
 *
 * O tempo medido e so o do calculo (omp_get_wtime, stderr); a impressao
 * fica fora, como no sequencial.
 */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(void) {
    int W, H, Imax;
    if (scanf("%d %d %d", &W, &H, &Imax) != 3) return 1;
    int *grid = malloc(sizeof(int) * (long)W * H);
    if (!grid) return 1;

    if (!getenv("OMP_SCHEDULE")) omp_set_schedule(omp_sched_dynamic, 1);

    double t0 = omp_get_wtime();
    #pragma omp parallel for schedule(runtime)
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++) {
            double cr = -2.0 + 3.0 * x / W;
            double ci = -1.5 + 3.0 * y / H;
            double zr = 0, zi = 0; int it = 0;
            while (it < Imax && zr*zr + zi*zi <= 4.0) {
                double t = zr*zr - zi*zi + cr;
                zi = 2*zr*zi + ci; zr = t; it++;
            }
            grid[(long)y*W+x] = it;
        }
    double t1 = omp_get_wtime();

    for (long i = 0; i < (long)W*H; i++) printf("%d\n", grid[i]);

    omp_sched_t tipo; int chunk;
    omp_get_schedule(&tipo, &chunk);
    tipo = (omp_sched_t)((int)tipo & 0x7fffffff);   /* tira o bit "monotonic" */
    const char *nome = tipo == omp_sched_static  ? "static"  :
                       tipo == omp_sched_dynamic ? "dynamic" :
                       tipo == omp_sched_guided  ? "guided"  : "auto";
    fprintf(stderr, "tempo: %.6f s (threads=%d, schedule=%s,%d)\n",
            t1 - t0, omp_get_max_threads(), nome, chunk);
    free(grid);
    return 0;
}
