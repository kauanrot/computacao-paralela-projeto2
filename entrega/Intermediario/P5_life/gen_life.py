#!/usr/bin/env python3
"""Gera a entrada de referencia do P5 (caderno, Apendice B.5).
N = 5000, G = 500, densidade 0.15, seed 42.
Uso: python3 gen_life.py > entrada.in
     python3 gen_life.py 1000 50 > pequeno.in   (N e G opcionais, para testes rapidos)
"""
import random, sys

def main():
    N = int(sys.argv[1]) if len(sys.argv) > 1 else 5000
    G = int(sys.argv[2]) if len(sys.argv) > 2 else 500
    random.seed(42)
    dens = 0.15
    out = sys.stdout.buffer            # bytes: sempre '\n', mesmo no Windows
    out.write(f"{N} {G}\n".encode())
    for _ in range(N):
        linha = ''.join('x' if random.random() < dens else ' ' for _ in range(N))
        out.write((linha + '\n').encode())

if __name__ == "__main__":
    main()
