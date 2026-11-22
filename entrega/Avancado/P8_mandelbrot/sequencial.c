/* P8 - Fractal de Mandelbrot (versao sequencial de referencia)
 *
 * Igual ao codigo base do caderno; a unica mudanca e a medicao de tempo do
 * calculo da grade (stderr). A impressao dos W*H inteiros fica fora da
 * medicao: e I/O serial e dominaria o tempo.
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double agora(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(void) {
    int W, H, Imax;
    if (scanf("%d %d %d", &W, &H, &Imax) != 3) return 1;
    int *grid = malloc(sizeof(int) * (long)W * H);
    if (!grid) return 1;

    double t0 = agora();
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
    double t1 = agora();

    for (long i = 0; i < (long)W*H; i++) printf("%d\n", grid[i]);
    fprintf(stderr, "tempo: %.6f s (sequencial)\n", t1 - t0);
    free(grid);
    return 0;
}
