/* P1 - Calculo de pi por integracao numerica (versao sequencial de referencia)
 *
 * Igual ao codigo base do caderno; a unica mudanca e a medicao de tempo,
 * que vai para stderr (a saida em stdout continua identica ao original).
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
    long long N;
    if (scanf("%lld", &N) != 1 || N <= 0) return 1;

    double step = 1.0 / (double)N, sum = 0.0;

    double t0 = agora();
    for (long long i = 0; i < N; i++) {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }
    double t1 = agora();

    printf("%.15f\n", step * sum);
    fprintf(stderr, "tempo: %.6f s (sequencial)\n", t1 - t0);
    return 0;
}
