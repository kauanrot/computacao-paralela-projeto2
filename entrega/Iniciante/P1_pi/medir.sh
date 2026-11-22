#!/usr/bin/env bash
# Mede o P1 seguindo o protocolo do caderno:
#   - P em {1, 2, 4, 6}, OMP_NUM_THREADS fixo
#   - 1 execucao de aquecimento descartada + 5 medidas, reporta a MEDIANA
#   - confere a corretude (|delta| relativo <= 1e-9 contra o sequencial)
# Gera resultados.csv (P, T, S, E) para montar a tabela e os graficos.
#
# Uso: ./medir.sh            (usa entrada.in)
#      ./medir.sh outra.in
set -euo pipefail
cd "$(dirname "$0")"

ENTRADA="${1:-entrada.in}"
REPS=5
THREADS="1 2 4 6"
export OMP_PROC_BIND=close   # prende as threads nos nucleos (medicao mais estavel)
export OMP_PLACES=cores

make -s

# tempo de 1 execucao (le a linha "tempo: X s" que o programa escreve no stderr)
tempo() { "$@" < "$ENTRADA" 2>&1 >/dev/null | awk '/^tempo:/ {print $2}'; }

# aquecimento + REPS execucoes -> mediana
mediana_de() {
    "$@" < "$ENTRADA" > /dev/null 2>&1      # warm-up, descartado
    for _ in $(seq "$REPS"); do tempo "$@"; done |
        sort -g | awk '{a[NR]=$1} END {print (NR%2) ? a[(NR+1)/2] : (a[NR/2]+a[NR/2+1])/2}'
}

echo "== Corretude"
SEQ=$(./sequencial < "$ENTRADA" 2>/dev/null)
for P in $THREADS; do
    PAR=$(OMP_NUM_THREADS=$P ./paralelo < "$ENTRADA" 2>/dev/null)
    awk -v a="$SEQ" -v b="$PAR" -v p="$P" 'BEGIN {
        d = (a - b) / a; if (d < 0) d = -d
        printf "  P=%d  seq=%s  par=%s  |delta|rel=%.2e  %s\n", p, a, b, d, (d <= 1e-9 ? "OK" : "FALHOU")
    }'
done

echo "== Tempos (mediana de $REPS, aquecimento descartado)"
TSEQ=$(mediana_de ./sequencial)
echo "  sequencial: ${TSEQ} s"

echo "P,T,S,E" > resultados.csv
T1=""
for P in $THREADS; do
    export OMP_NUM_THREADS=$P
    T=$(mediana_de ./paralelo)
    [ -z "$T1" ] && T1=$T
    awk -v p="$P" -v t="$T" -v t1="$T1" -v ts="$TSEQ" 'BEGIN {
        s = t1 / t; e = s / p
        printf "  P=%d  T=%.4f s  S=%.2f  E=%.2f  (S vs sequencial=%.2f)\n", p, t, s, e, ts / t
        printf "%d,%.6f,%.4f,%.4f\n", p, t, s, e >> "resultados.csv"
    }'
done
echo "== Salvo em resultados.csv"
