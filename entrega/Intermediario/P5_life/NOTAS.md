# P5 — Jogo da Vida: notas para o relatório

## Como rodar (Linux/WSL/Codespace)

```bash
make                            # gera ./sequencial e ./paralelo (e entrada.in, se faltar)
./sequencial < entrada.in > saida.txt          # tabuleiro final em stdout, tempo em stderr
OMP_NUM_THREADS=4 ./paralelo < entrada.in > saida.txt
./medir.sh                      # protocolo completo -> resultados.csv
python3 gen_life.py 1000 50 > pequeno.in       # entrada menor para testes rápidos
THREADS="1 2 4" ./medir.sh pequeno.in          # ex.: Codespace de 4 vCPU
```

`entrada.in` é a entrada de referência do caderno (Apêndice B.5): N = 5000, G = 500,
densidade 0,15, seed 42, gerada pelo `gen_life.py`.

## 1. Contexto
Grade N × N, G gerações, regras de Conway (vizinhança de Moore). Custo O(N²·G): na
referência são 5000² × 500 = 1,25·10¹⁰ atualizações de célula, cada uma lendo 9 células.
Os tabuleiros (2 × 25 MB) não cabem no cache L3 (32 MB no 5600X dividido com o resto), então
o problema é limitado em parte pela memória, não só pela CPU.

## 2. Correção no sequencial (declarar no relatório)
O código base lê cada linha com `fgets` num buffer reaproveitado e usa `line[i]` para
i = 0..N-1 sem olhar onde a linha terminou. Se uma linha vier mais curta que N (por exemplo,
um editor que apaga os espaços no fim da linha), o programa lê o `'\n'`, o `'\0'` e **o que
sobrou da linha anterior**. Teste com um tabuleiro 4×4 em que a 2ª linha é só `x` (G = 0, a
saída deveria repetir a entrada):

| | linha 2 |
|---|---|
| original | `x..x` (o `x` do fim veio da linha 1) |
| corrigido | `x...` |

Correção: `len = strcspn(line, "\r\n")` e posições `i >= len` contam como célula morta.
Para entradas bem formadas (como a do `gen_life.py`) a saída é idêntica à do original;
conferido contra o `life.expected` do caderno e contra o código base em entradas geradas.
Como o algoritmo não mudou, o speedup não precisa de rebase.

## 3. Estratégia de paralelização
- **Dependência:** a geração t+1 só lê a geração t (dois tabuleiros, `cur` e `nxt`). Dentro
  de uma geração, cada célula de `nxt` é escrita por uma única iteração e `cur` só é lido,
  então as células são independentes. **Entre** gerações há dependência: a t+1 só pode
  começar quando a t terminou inteira.
- **Uma região paralela para todas as gerações** (`#pragma omp parallel` fora do laço `s`):
  as threads são criadas uma vez, não G = 500 vezes.
- **`#pragma omp for` nas colunas `i` a cada geração.** A barreira implícita no fim do
  `omp for` é exatamente a dependência entre gerações: nenhuma thread troca os tabuleiros
  antes de todas terminarem a geração atual.
- **Troca `cur`/`nxt` sem sincronização extra:** cada thread tem cópias locais dos
  ponteiros (`c`, `p`) e todas fazem a mesma troca. Não precisa de `single` nem de uma
  segunda barreira. No fim, o tabuleiro final é `cur` se G for par e `nxt` se for ímpar.
- **`schedule(static)`:** toda célula custa o mesmo (9 leituras), então blocos fixos e
  contíguos de colunas equilibram a carga e cada thread percorre a memória em sequência.
- **Sem `reduction`, `atomic` ou `critical`:** nenhuma posição é escrita por duas threads.
- **Não paralelizado:** leitura e impressão (I/O serial, fora da medição de tempo).
- O laço paralelizado é `i` (externo). Paralelizar `j` (interno) daria N fork/joins por
  geração com pouco trabalho cada.

## 4/5. Tabela e gráficos
Preencher com `resultados.csv` gerado pelo `medir.sh` na entrada de referência.

## 6. Pontos para a análise
- **Resultado é exato:** inteiros/caracteres, a saída paralela tem que ser idêntica byte a
  byte à sequencial (o `medir.sh` confere com hash da saída).
- **Gargalo esperado: memória.** Cada geração percorre 2 × 25 MB. Com mais threads, a banda
  de memória (compartilhada) satura antes dos núcleos; espere eficiência caindo com P.
  Comparar com o P1 (CPU puro, E ≈ 1) é um bom argumento na análise.
- **Barreira por geração:** 500 barreiras. Custo pequeno perto de ~0,2 s por geração, mas
  qualquer desequilíbrio entre threads (turbo diferente por núcleo, outro processo) vira
  espera em todas as gerações.
- **Amdahl:** a parte serial (ler 25 MB, imprimir 25 MB) não está no tempo medido; se
  medisse o programa inteiro, o I/O limitaria o speedup.
- **Experimento extra (opcional):** `#pragma omp parallel for` dentro do laço de gerações
  (cria a região 500 vezes) vs a versão atual; ou `collapse(2)`.
