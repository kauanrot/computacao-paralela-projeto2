#include <stdio.h>
#include <stdlib.h>
int main(void) {
long long N;
if (scanf("%lld", &N) != 1 || N <= 0) return 1;
double step = 1.0 / (double)N, sum = 0.0;
for (long long i = 0; i < N; i++) {
double x = (i + 0.5) * step;
sum += 4.0 / (1.0 + x * x);
}
printf("%.15f\n", step * sum);
return 0;
}
