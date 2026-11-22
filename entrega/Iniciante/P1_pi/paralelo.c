/* P1 - Calculo de pi por integracao numerica (versao paralela com OpenMP)
 *
 * Estrategia:
 *  - Cada iteracao i so depende de i (x = (i + 0.5) * step), entao as
 *    iteracoes sao independentes e podem ser divididas entre as threads.
 *  - A unica variavel compartilhada que e escrita e "sum". Escrever nela
 *    direto de varias threads seria condicao de corrida (sum += ... nao e
 *    atomico: le, soma, escreve). Por isso: reduction(+:sum), que da a cada
 *    thread uma copia privada (iniciada em 0) e soma as copias no final.
 *  - schedule(static): todas as iteracoes custam o mesmo, entao blocos fixos
 *    e contiguos bastam e nao tem custo de distribuicao em tempo de execucao.
 *  - x e declarado dentro do laco, entao ja e privado de cada thread.
 *
 * O tempo medido e so o do laco (omp_get_wtime), vai para stderr.
 */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(void) {
    long long N;
    if (scanf("%lld", &N) != 1 || N <= 0) return 1;

    double step = 1.0 / (double)N, sum = 0.0;

    double t0 = omp_get_wtime();
    #pragma omp parallel for reduction(+:sum) schedule(static)
    for (long long i = 0; i < N; i++) {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }
    double t1 = omp_get_wtime();

    printf("%.15f\n", step * sum);
    fprintf(stderr, "tempo: %.6f s (threads=%d)\n", t1 - t0, omp_get_max_threads());
    return 0;
}
