# Projeto 2 — Desafios de Computação Paralela (OpenMP)

Enunciado completo em `enunciado/`. Códigos base originais em `caderno-teste/` (não editar; serve de referência e para o `test.sh`).

## Estrutura

```
caderno-teste/   codigos sequenciais originais + test.sh (nao mexer)
enunciado/       PDF do caderno
entrega/
  Iniciante/     P1_pi/ ou P2_threshold/
  Intermediario/ P4_histogram/, P5_life/, P6_sieve/ ou P7_mergesort/
  Avancado/      P8_mandelbrot/, P9_nqueens/ ou P10_floyd/
  relatorio/     relatorio em PDF + graficos
```

Cada pasta de problema escolhido deve ter: `sequencial.c`, `paralelo.c`, `Makefile`, `entrada.in`.

## O que entregar

- 3 problemas, **um de cada nível** (escolha livre dentro do nível).
- Warm-ups W1, W2, W3: cada membro faz individualmente (pré-requisito, não entram no relatório).
- Medir com P = 1, 2, 4, 6 threads; mediana de 5 execuções, descartando a 1ª; `OMP_NUM_THREADS` fixo.
- Tabela T(P), S(P), E(P) + 2 gráficos (S×P com reta y=P; E×P com reta y=1).
- Relatório: um capítulo de 4–6 páginas por problema (contexto, estratégia, tabela, gráficos, análise, contribuição).
- Nota: corretude 30%, paralelização 30%, documentação 15%, apresentação 25%.

## Ambiente

- CPU: Ryzen 5 5600X (6 núcleos físicos / 12 lógicos) — P=6 sem hyperthreading.
- Compilar: `gcc -O3 -march=native -fopenmp prog.c -o prog`.
- Caderno recomenda Linux nativo; se usar WSL2, declarar no relatório.
- **Codespace** (`.devcontainer/`): máquina de 4 vCPU com gcc + OpenMP, para demonstrar na apresentação.
  Abrir em *Code → Codespaces → Create codespace on main*. Os 4 vCPU são threads (≈2 núcleos físicos)
  e a VM é compartilhada, então os tempos do relatório saem do 5600X, não do Codespace.

## Pontos encontrados nos códigos base

1. Makefiles do caderno **não têm `-fopenmp`**.
2. `test.sh` compara com `diff` exato: P1 paralelo vai dar DIFF no último dígito (tolerância real é 1e-9).
   O `pi.expected` é `3.141592653589971`, o PDF mostra `...589793`.
3. W1: `sum.expected` = `7.4854708606`, PDF diz `...605` (o código arredonda; o arquivo está certo).
4. P10 Floyd: `dik + dkj` pode estourar `int` (V=2000, pesos até 1e6) — somar em `long long`.
5. P5 Life: se uma linha vier mais curta que N, o `fgets` deixa lixo da linha anterior no buffer.
6. P6 Crivo: o laço externo precisa ir até N (é nele que se conta os primos); só a marcação dos múltiplos
   para depois de √N. Com `long` de 32 bits (Windows), `i * i` estoura e o `j > 0` depende de comportamento indefinido.
7. P2, P4, P7, P8: I/O (`scanf`/`printf`) domina o tempo e é serial — medir só o kernel com `omp_get_wtime()`.
   P4 precisa armazenar os pixels (ex.: `unsigned char`, ~1 GB para M=1e9) para paralelizar.
8. W1: a soma é em inteiros, então a ordem não muda o resultado (truncagem por termo independe da ordem).

## Problemas escolhidos

- Iniciante: **P1 — π** (`entrega/Iniciante/P1_pi/`)
- Intermediário: **P5 — Jogo da Vida** (`entrega/Intermediario/P5_life/`)
- Avançado: **P8 — Mandelbrot** (`entrega/Avancado/P8_mandelbrot/`)

Cada pasta tem `NOTAS.md` (rascunho do capítulo do relatório) e `medir.sh` (protocolo de medição).
