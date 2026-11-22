#!/usr/bin/env bash
# Compara politicas de escalonamento do P8 com P fixo (padrao: 6 threads).
# O paralelo.c usa schedule(runtime), entao a politica vem de OMP_SCHEDULE,
# sem recompilar. Mesmo protocolo do medir.sh (aquecimento + mediana).
#
# Uso: ./comparar_schedule.sh                 (entrada.in, P=6)
#      P=4 REPS=3 ./comparar_schedule.sh pequeno.in
#      SCHEDULES="static dynamic,1" ./comparar_schedule.sh
set -euo pipefail
cd "$(dirname "$0")"

ENTRADA="${1:-entrada.in}"
REPS="${REPS:-5}"
P="${P:-6}"
SCHEDULES="${SCHEDULES:-static static,1 dynamic,1 dynamic,16 guided}"
export OMP_PROC_BIND=close OMP_PLACES=cores OMP_NUM_THREADS="$P"

make -s paralelo
[ -f "$ENTRADA" ] || { echo "ERRO: entrada '$ENTRADA' nao existe" >&2; exit 1; }

tempo() {
    local t; t=$(./paralelo < "$ENTRADA" 2>&1 >/dev/null | awk '/^tempo:/ {print $2}')
    [ -n "$t" ] || { echo "ERRO: paralelo nao informou o tempo" >&2; return 1; }
    echo "$t"
}

echo "== Entrada: $ENTRADA | P=$P | mediana de $REPS (aquecimento descartado)"
echo "schedule,T" > schedules.csv
for SC in $SCHEDULES; do
    export OMP_SCHEDULE="$SC"
    tempo > /dev/null                                   # aquecimento
    tempos=(); for _ in $(seq "$REPS"); do t=$(tempo); tempos+=("$t"); done
    T=$(printf '%s\n' "${tempos[@]}" | sort -g |
        awk '{a[NR]=$1} END {print (NR%2) ? a[(NR+1)/2] : (a[NR/2]+a[NR/2+1])/2}')
    printf "  %-12s T=%.4f s\n" "$SC" "$T"
    echo "$SC,$T" >> schedules.csv
done
echo "== Salvo em schedules.csv"
