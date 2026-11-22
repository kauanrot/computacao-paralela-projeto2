# P8 — Fractal de Mandelbrot: notas para o relatório

## Como rodar (Linux/WSL/Codespace)

```bash
make                                    # gera ./sequencial e ./paralelo
./sequencial < entrada.in > saida.txt   # W*H inteiros em stdout, tempo em stderr
OMP_NUM_THREADS=4 ./paralelo < entrada.in > saida.txt
OMP_SCHEDULE=static OMP_NUM_THREADS=4 ./paralelo < entrada.in > /dev/null
./medir.sh                              # protocolo completo -> resultados.csv
./comparar_schedule.sh                  # static x dynamic x guided com P=6 -> schedules.csv
echo "4000 4000 4000" > medio.in        # entrada menor para testes rápidos
THREADS="1 2 4" ./medir.sh medio.in     # ex.: Codespace de 4 vCPU
```

`entrada.in` é a entrada de referência do caderno: W = H = 10⁴, Imax = 10⁴.
**Atenção ao tempo:** na referência o cálculo leva ~1 min com 6 threads (MSVC, 5600X), então
o protocolo completo (aquecimento + 5 medidas, sequencial + 4 valores de P) passa de 1 h.
A saída tem 10⁸ linhas (~600 MB); redirecionar para arquivo ou `/dev/null`.

## 1. Contexto
Para cada pixel de uma grade W × H mapeada em [-2, 1] × [-1,5; 1,5], itera z ← z² + c até
|z| > 2 ou Imax iterações. Custo no pior caso W·H·Imax = 10¹² na referência; na prática bem
menos, porque a maioria dos pixels escapa em poucas iterações. CPU-bound: a grade (400 MB)
só é escrita uma vez por pixel.

## 2. Mudanças no sequencial
Só a medição de tempo (stderr) em volta do cálculo. A saída é idêntica ao código base
(conferido contra o `mandelbrot.expected` do caderno e contra o código base em 1200×900).

## 3. Estratégia de paralelização
- **Sem dependência entre pixels:** cada pixel só lê (x, y, W, H, Imax) e escreve a própria
  posição de `grid`. Nenhuma variável compartilhada é escrita por duas threads: sem
  `reduction`, `atomic` ou `critical`. `cr`, `ci`, `zr`, `zi`, `it`, `t` são declaradas dentro
  do laço, então são privadas.
- **`parallel for` nas linhas (y):** uma linha (W = 10⁴ pixels) é uma unidade de trabalho
  grande o bastante para o overhead de distribuição ser desprezível, e cada thread escreve
  um trecho contíguo de `grid`.
- **O problema é o balanceamento de carga.** Um pixel dentro do conjunto custa Imax
  iterações; um longe escapa em 1–2. As linhas do meio da imagem atravessam o conjunto e
  custam muito mais que as das bordas. Com `schedule(static)` cada thread recebe um bloco
  contíguo de linhas: as threads do meio trabalham muito mais e as outras ficam paradas na
  barreira do fim.
- **`schedule(runtime)`, padrão `dynamic,1`:** as threads pegam a próxima linha livre assim
  que terminam a atual, o que equilibra a carga. O `runtime` permite comparar políticas
  só mudando `OMP_SCHEDULE`, sem recompilar (é o que o `comparar_schedule.sh` faz). Se
  `OMP_SCHEDULE` não estiver definida, o programa usa `dynamic,1` (`omp_set_schedule`).
- **Não paralelizado:** impressão dos W·H inteiros (I/O serial, fora da medição).

## 4/5. Tabela e gráficos
Preencher com `resultados.csv` (`medir.sh`). Para a análise de balanceamento, um gráfico
extra de barras com `schedules.csv` (`comparar_schedule.sh`).

## 6. Pontos para a análise
- **Resultado é exato:** inteiros, saída paralela idêntica byte a byte à sequencial para
  qualquer P e qualquer schedule (conferido; o `medir.sh` confere com hash).
- **static × dynamic:** é o ponto central do capítulo. Com static, o speedup fica preso à
  thread com o bloco mais caro; com dynamic ele fica perto de P.
- **`static,1` (round-robin)** também equilibra bem, porque linhas vizinhas têm custo
  parecido e são espalhadas entre as threads; vale mostrar que não é só "dynamic resolve".
- **Amdahl:** a parte serial medida é ~0 (só o cálculo é cronometrado). Se medisse o
  programa inteiro, imprimir 10⁸ linhas dominaria o tempo.
- **Saturação:** CPU-bound, sem memória compartilhada disputada; a perda de eficiência com
  P = 6 deve vir de turbo menor com mais núcleos ativos, não de memória.
