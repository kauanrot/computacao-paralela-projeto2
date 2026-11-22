#!/usr/bin/env bash
set -uo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
FAIL=0

# $1 = pasta, $2 = nome do binario (igual ao nome da pasta), $3 = entrada, $4 = expected
run_c() {
    local dir="$1" bin="$2" in="$3" exp="$4"
    echo "== $dir =="
    ( cd "$dir" && make -s ) || { echo "  COMPILE FAIL"; FAIL=1; return; }
    if [ ! -f "$dir/$exp" ]; then
        ( cd "$dir" && ./$bin < "$in" > "$exp" )
        echo "  (gerado $exp na primeira execucao)"
        return
    fi
    if diff -q <(cd "$dir" && ./$bin < "$in") "$dir/$exp" >/dev/null; then
        echo "  OK"
    else
        echo "  DIFF"; FAIL=1
    fi
}

run_c warmups/W1_harmonic       W1_harmonic       sum.in          sum.expected
run_c warmups/W2_sudokount      W2_sudokount      sudokount1.in   sudokount1.expected
run_c warmups/W3_matmul         W3_matmul         matmul.in       matmul.expected
run_c iniciante/P1_pi           P1_pi             pi.in           pi.expected
run_c iniciante/P2_threshold    P2_threshold      threshold.in    threshold.expected
run_c intermediario/P4_histogram P4_histogram     histogram.in    histogram.expected
run_c intermediario/P5_life     P5_life           life.in         life.expected
run_c intermediario/P6_sieve    P6_sieve          sieve.in        sieve.expected
run_c intermediario/P7_mergesort P7_mergesort     mergesort.in    mergesort.expected
run_c avancado/P8_mandelbrot    P8_mandelbrot     mandelbrot.in   mandelbrot.expected
run_c avancado/P9_nqueens       P9_nqueens        nqueens.in      nqueens.expected
run_c avancado/P10_floyd        P10_floyd         floyd.in        floyd.expected
run_c bonus/parser              parser            spec.in         spec.expected

exit $FAIL
