#!/usr/bin/env bash
# Roda a versao rapida dos tres problemas (entradas pequenas), um depois do outro:
# compila, confere a corretude contra o sequencial e mede T, S, E.
# Usa P = 1, 2, 4, 6 ate o numero de nucleos da maquina.
#
# Uso: ./rapido.sh
set -euo pipefail
cd "$(dirname "$0")/entrega"
for d in Iniciante/P1_pi Intermediario/P5_life Avancado/P8_mandelbrot; do
    echo
    echo "################ $d"
    make -s -C "$d" rapido
done
