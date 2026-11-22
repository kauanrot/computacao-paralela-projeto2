#!/usr/bin/env bash
# Confere os codigos base ORIGINAIS do caderno (P1, P5, P8) contra as saidas esperadas.
# Serve de referencia: mostra que o ambiente compila e roda os originais corretamente.
# (Os codigos da entrega ficam em ../entrega e sao conferidos pelo medir.sh de cada pasta.)
#
# Uso: bash test.sh
set -uo pipefail
cd "$(dirname "$0")"
FAIL=0

# $1 = pasta, $2 = binario (nome da pasta), $3 = entrada, $4 = esperado
run_c() {
    local dir="$1" bin="$2" in="$3" exp="$4"
    echo "== $dir =="
    ( cd "$dir" && make -s ) || { echo "  COMPILE FAIL"; FAIL=1; return; }
    if diff -q <(cd "$dir" && ./$bin < "$in") "$dir/$exp" >/dev/null; then
        echo "  OK"
    else
        echo "  DIFF"; FAIL=1
    fi
}

run_c iniciante/P1_pi        P1_pi        pi.in         pi.expected
run_c intermediario/P5_life  P5_life      life.in       life.expected
run_c avancado/P8_mandelbrot P8_mandelbrot mandelbrot.in mandelbrot.expected

exit $FAIL
