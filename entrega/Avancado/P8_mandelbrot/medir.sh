#!/usr/bin/env bash
# Mede o programa seguindo o protocolo do caderno:
#   - P em {1, 2, 4, 6}, OMP_NUM_THREADS fixo
#   - 1 execucao de aquecimento descartada + 5 medidas, reporta a MEDIANA
#   - S(P) = T(1)/T(P) e E(P) = S(P)/P (definicao do caderno)
#   - corretude: a saida (stdout) de cada P tem que ser identica a do
#     sequencial (inteiros/caracteres, comparacao exata). A saida conferida e
#     a do aquecimento, entao a conferencia nao custa execucoes extras.
# Gera resultados.csv (P, T, S, E, S contra o sequencial, correto).
#
# Uso: ./medir.sh                   (entrada.in)
#      ./medir.sh outra.in
#      THREADS="1 2 4" REPS=3 ./medir.sh pequeno.in   (ex.: Codespace de 4 vCPU)
set -euo pipefail
cd "$(dirname "$0")"

ENTRADA="${1:-entrada.in}"
REPS="${REPS:-5}"
THREADS="${THREADS:-1 2 4 6}"
export OMP_PROC_BIND=close   # prende as threads nos nucleos (medicao mais estavel)
export OMP_PLACES=cores

make -s
make -s "$ENTRADA" 2>/dev/null || true      # gera entrada.in se houver regra
[ -f "$ENTRADA" ] || { echo "ERRO: entrada '$ENTRADA' nao existe" >&2; exit 1; }

TMP=$(mktemp -d); trap 'rm -rf "$TMP"' EXIT

# 1 execucao -> "tempo hash_da_saida" (o programa escreve "tempo: X s" no stderr)
rodar() {
    "$@" < "$ENTRADA" 2> "$TMP/err" | tr -d '\r' | md5sum | cut -d' ' -f1 > "$TMP/hash"
    local t; t=$(awk '/^tempo:/ {print $2}' "$TMP/err")
    if [ -z "$t" ]; then echo "ERRO: '$*' nao informou o tempo:" >&2; cat "$TMP/err" >&2; return 1; fi
    echo "$t $(cat "$TMP/hash")"
}

# aquecimento (tempo descartado, saida usada na corretude) + REPS medidas -> "mediana hash"
medir() {
    local r h tempos=()
    r=$(rodar "$@"); h=${r#* }
    for _ in $(seq "$REPS"); do r=$(rodar "$@"); tempos+=("${r%% *}"); done
    local med; med=$(printf '%s\n' "${tempos[@]}" | sort -g |
        awk '{a[NR]=$1} END {print (NR%2) ? a[(NR+1)/2] : (a[NR/2]+a[NR/2+1])/2}')
    echo "$med $h"
}

echo "== Entrada: $ENTRADA | threads: $THREADS | mediana de $REPS (aquecimento descartado)"
R=$(medir ./sequencial); read -r TSEQ HSEQ <<< "$R"
printf "  sequencial  T=%.4f s\n" "$TSEQ"

echo "P,T,S,E,S_seq,correto" > resultados.csv
T1=""; FALHA=0
for P in $THREADS; do
    export OMP_NUM_THREADS=$P
    R=$(medir ./paralelo); read -r T H <<< "$R"
    [ -z "$T1" ] && T1=$T
    OK=sim; [ "$H" = "$HSEQ" ] || { OK=NAO; FALHA=1; }
    awk -v p="$P" -v t="$T" -v t1="$T1" -v ts="$TSEQ" -v ok="$OK" 'BEGIN {
        s = t1 / t; e = s / p
        printf "  P=%d  T=%.4f s  S=%.2f  E=%.2f  (S vs sequencial=%.2f)  saida igual ao seq: %s\n", p, t, s, e, ts / t, ok
        printf "%d,%.6f,%.4f,%.4f,%.4f,%s\n", p, t, s, e, ts / t, ok >> "resultados.csv"
    }'
done
echo "== Salvo em resultados.csv"
[ "$FALHA" = 0 ] || { echo "ATENCAO: alguma saida paralela difere do sequencial" >&2; exit 1; }
