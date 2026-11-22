# Projeto 2 — Desafios de Computação Paralela (OpenMP)

Paralelização com OpenMP de três problemas do caderno, um de cada nível:

| Nível | Problema | Pasta |
|---|---|---|
| Iniciante | **P1 — Cálculo de π** (redução) | `entrega/Iniciante/P1_pi/` |
| Intermediário | **P5 — Jogo da Vida** (estêncil, barreira por geração) | `entrega/Intermediario/P5_life/` |
| Avançado | **P8 — Mandelbrot** (balanceamento de carga, `schedule`) | `entrega/Avancado/P8_mandelbrot/` |

Todos os testes e medições rodam no **GitHub Codespaces**, para que todo o grupo use o mesmo ambiente.

## Estrutura

```
.devcontainer/          configuracao do Codespace (gcc, OpenMP, python3, matplotlib)
rapido.sh               roda o teste rapido dos tres problemas
entrega/
  Iniciante/P1_pi/          sequencial.c  paralelo.c  Makefile  entrada.in  medir.sh
  Intermediario/P5_life/    sequencial.c  paralelo.c  Makefile  entrada.in  medir.sh  gen_life.py
  Avancado/P8_mandelbrot/   sequencial.c  paralelo.c  Makefile  entrada.in  medir.sh  medicao.in
                            comparar_schedule.sh
  relatorio/                relatorio.docx  graficos.py
caderno-teste/          codigos ORIGINAIS do caderno (P1, P5, P8) + test.sh (nao editar)
enunciado/              PDF do caderno
```

Em cada pasta de problema:

| Arquivo | Para que serve |
|---|---|
| `sequencial.c` | código base do caderno + medição de tempo (no P5, também a correção da leitura) |
| `paralelo.c` | versão OpenMP; os comentários no topo explicam a estratégia |
| `Makefile` | `make` compila; `make rapido` testa rápido; `make medir` faz a medição do relatório |
| `entrada.in` | entrada de referência do caderno |
| `medir.sh` | protocolo do caderno: aquecimento + mediana de 5, P = 1, 2, 4, 6, confere a corretude e gera `resultados.csv` |

---

## 1. Abrir o Codespace

1. No GitHub do repositório: botão verde **Code** → aba **Codespaces** → **Create codespace on main**.
2. Espere a instalação terminar (2 a 3 minutos na primeira vez). O Codespace já vem com gcc, OpenMP, python3 e matplotlib, numa máquina de 4 núcleos.
3. Abra o terminal (menu ☰ → *Terminal → New Terminal*, ou `` Ctrl+` ``) e confira:

```bash
gcc --version | head -1; nproc
```

Tem que aparecer a versão do gcc e `4`.

Se o Codespace já existia antes, atualize o código:

```bash
cd /workspaces/computacao-paralela-projeto2 && git pull
```

> Se o Codespace foi criado antes de o `.devcontainer` incluir o python3, instale na mão:
> `sudo apt-get update && sudo apt-get install -y python3 python3-matplotlib`

## 2. Teste rápido de tudo (1 comando)

Na raiz do repositório:

```bash
./rapido.sh
```

Compila os três problemas, gera entradas pequenas, confere se o paralelo dá a mesma saída do sequencial e mostra a tabela T, S, E de cada um. Leva poucos minutos.

O mesmo, um problema por vez, dentro da pasta dele: `make rapido`.

---

## 3. P1 — Cálculo de π, passo a passo

```bash
cd /workspaces/computacao-paralela-projeto2/entrega/Iniciante/P1_pi
```

**Compilar:**

```bash
make
```

**Rodar o sequencial** (imprime π; o tempo vai para a linha `tempo:`):

```bash
./sequencial < entrada.in
```

**Rodar o paralelo com 4 threads:**

```bash
OMP_NUM_THREADS=4 ./paralelo < entrada.in
```

O resultado muda no último dígito em relação ao sequencial. Isso é esperado: com várias threads a soma é feita em outra ordem, e a diferença (~10⁻¹⁴) fica muito abaixo da tolerância do caderno (10⁻⁹).

**Medição completa** (confere a corretude com tolerância 10⁻⁹ e gera `resultados.csv`):

```bash
make medir
```

## 4. P5 — Jogo da Vida, passo a passo

```bash
cd /workspaces/computacao-paralela-projeto2/entrega/Intermediario/P5_life
```

**Compilar:**

```bash
make
```

**Ver funcionando num exemplo pequeno** (3 células em pé, 1 geração — elas deitam):

```bash
printf '3 1\n x \n x \n x \n' | ./sequencial
```

```bash
printf '3 1\n x \n x \n x \n' | OMP_NUM_THREADS=4 ./paralelo
```

Os dois têm que mostrar a linha do meio cheia (`xxx`).

**Gerar uma entrada média** (1000 × 1000, 100 gerações). A entrada é um tabuleiro inteiro, por isso é gerada pelo script e não digitada:

```bash
python3 gen_life.py 1000 100 > pequeno.in
```

**Rodar e comparar as saídas** (o tabuleiro final vai para arquivo, senão enche a tela):

```bash
./sequencial < pequeno.in > saida_seq.txt
```

```bash
OMP_NUM_THREADS=4 ./paralelo < pequeno.in > saida_par.txt
```

```bash
cmp saida_seq.txt saida_par.txt && echo IGUAIS
```

**Medição completa** na entrada de referência (5000 × 5000, 500 gerações; leva cerca de 1 hora no Codespace):

```bash
make medir
```

## 5. P8 — Mandelbrot, passo a passo

```bash
cd /workspaces/computacao-paralela-projeto2/entrega/Avancado/P8_mandelbrot
```

**Compilar:**

```bash
make
```

**Ver funcionando no exemplo do enunciado** (imagem 3 × 3, Imax 50 — imprime 9 números):

```bash
echo "3 3 50" | ./sequencial
```

```bash
echo "3 3 50" | OMP_NUM_THREADS=4 ./paralelo
```

**Rodar e comparar numa entrada maior** (a saída tem milhões de linhas, então vai para arquivo):

```bash
echo "2000 2000 2000" > pequeno.in
```

```bash
./sequencial < pequeno.in > saida_seq.txt
```

```bash
OMP_NUM_THREADS=4 ./paralelo < pequeno.in > saida_par.txt
```

```bash
cmp saida_seq.txt saida_par.txt && echo IGUAIS
```

**Ver a diferença entre os schedules** (o ponto principal do P8):

```bash
OMP_SCHEDULE=static OMP_NUM_THREADS=4 ./paralelo < pequeno.in > /dev/null
```

```bash
OMP_SCHEDULE=dynamic,1 OMP_NUM_THREADS=4 ./paralelo < pequeno.in > /dev/null
```

O `dynamic` é bem mais rápido: as linhas do meio da imagem custam muito mais que as das bordas, e com `static` a thread que pega o meio trabalha sozinha no fim.

**Medição completa** (P = 1, 2, 4, 6 e comparação de schedules, na `medicao.in` = 4000 × 4000, Imax 4000):

```bash
make medir
```

A `entrada.in` (10⁴ × 10⁴, Imax 10⁴) é a de referência do caderno, mas o protocolo completo nela levaria horas no Codespace; por isso a medição usa a `medicao.in`, e isso está declarado no relatório.

---

## 6. Medição para o relatório

1. **Aumente o tempo de inatividade do Codespace** antes das medições longas: em github.com/settings/codespaces, *Default idle timeout* = 240 minutos. Com o padrão de 30 minutos, o Codespace pode desligar no meio do P5.
2. Rode `make medir` em cada pasta (seções 3, 4 e 5). Cada uma gera um `resultados.csv` (e o P8 também `schedules.csv`).
3. Anote o ambiente para a tabela do relatório:

```bash
lscpu | grep -E 'Model name|Core|Thread|Socket|L3'; gcc --version | head -1
```

4. Gere os gráficos e as tabelas (saem em `entrega/relatorio/graficos/`):

```bash
cd /workspaces/computacao-paralela-projeto2 && python3 entrega/relatorio/graficos.py
```

5. Commite os resultados, para o grupo todo ter os números:

```bash
git add entrega/*/*/resultados.csv entrega/Avancado/P8_mandelbrot/schedules.csv && git commit -m "Resultados das medicoes" && git push
```

## 7. Relatório

`entrega/relatorio/relatorio.docx` segue o template do Apêndice C (um capítulo por problema: contexto, estratégia, tabela, gráficos, análise, contribuição). O texto está escrito; **o que está destacado em amarelo falta preencher**:

- capa (universidade, curso, integrantes, professor, data);
- tabela de ambiente (saída do `lscpu` e do `gcc --version`);
- tabelas de medição (copiar de `entrega/relatorio/graficos/*_tabela.txt`);
- figuras (inserir os PNGs de `entrega/relatorio/graficos/` no lugar das caixas tracejadas);
- trechos da análise que dependem dos números medidos;
- seção 6 de cada capítulo: um parágrafo por integrante.

Depois: atualizar o sumário (botão direito → *Atualizar campo*) e exportar para PDF (*Arquivo → Salvar como → PDF*).

## 8. Conferir os códigos originais do caderno

Roda os códigos base originais de P1, P5 e P8 contra as saídas esperadas do caderno:

```bash
cd /workspaces/computacao-paralela-projeto2/caderno-teste && bash test.sh
```

---

## Observações sobre os códigos base

- **Compilação:** os Makefiles do caderno não têm `-fopenmp`; os da entrega têm (só no `paralelo`).
- **P1:** o `test.sh` compara com `diff` exato, então o P1 paralelo daria `DIFF` no último dígito mesmo correto. O `medir.sh` compara com tolerância relativa de 10⁻⁹, como pede o caderno.
- **P5:** se uma linha da entrada vier mais curta que N, o código base lê o que sobrou da linha anterior no buffer do `fgets`. A versão da entrega trata o fim da linha como célula morta (documentado no relatório). Para entradas bem formadas a saída é idêntica.
- **P1, P5, P8:** só o cálculo é cronometrado (`omp_get_wtime()`); ler a entrada e imprimir a saída é I/O serial e dominaria o tempo.

## Regras de entrega (resumo do caderno)

- 3 problemas, **um de cada nível**; cada pasta com `sequencial.c`, `paralelo.c`, `Makefile`, `entrada.in`.
- Medir com P = 1, 2, 4, 6; mediana de 5 execuções, descartando a 1ª; `OMP_NUM_THREADS` fixo.
- Tabela T(P), S(P) = T(1)/T(P), E(P) = S(P)/P + 2 gráficos (S × P com a reta y = P; E × P com a reta y = 1).
- Relatório em PDF: um capítulo de 4–6 páginas por problema.
- Warm-ups W1, W2, W3: cada membro faz individualmente (pré-requisito, não entram no relatório).
- Apresentação: 10 min + 2 de perguntas, sobre um dos três problemas, escolhido pelo professor; qualquer membro pode ser chamado para explicar.
- Nota: corretude 30%, paralelização 30%, documentação 15%, apresentação 25%.
