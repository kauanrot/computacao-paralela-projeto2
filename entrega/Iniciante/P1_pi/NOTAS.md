# P1 — Cálculo de π: notas para o relatório

## Como rodar (Linux/WSL)

```bash
make                       # gera ./sequencial e ./paralelo
./sequencial < entrada.in  # pi em stdout, tempo em stderr
OMP_NUM_THREADS=4 ./paralelo < entrada.in
./medir.sh                 # protocolo completo -> resultados.csv
```

## 1. Contexto
Soma de N = 10⁹ termos de 4/(1+x²), x = (i+0.5)/N. Custo O(N), CPU-bound (uma divisão por
iteração), quase sem acesso à memória: os dados cabem nos registradores.

## 2. Estratégia de paralelização
- **O que foi paralelizado:** o único laço (`for i`). Cada iteração só depende de `i`,
  então não há dependência entre iterações.
- **Condição de corrida:** `sum += ...` em várias threads ao mesmo tempo perde somas
  (ler–somar–escrever não é atômico). Resolvido com `reduction(+:sum)`: cópia privada por
  thread, iniciada em 0, combinada no final.
- **Por que não `atomic`/`critical`:** seriam 10⁹ operações sincronizadas; todas as threads
  disputariam a mesma variável e ficaria mais lento que o sequencial.
- **`schedule(static)`:** o custo de cada iteração é igual, então blocos fixos e contíguos
  dividem a carga por igual sem overhead de distribuição.
- **`x`** é declarado dentro do laço, portanto já é privado.
- **O que não foi paralelizado:** leitura de N e `printf` (custo desprezível).
- **Mudanças no sequencial:** só a medição de tempo (stderr). A saída é idêntica à do original.

## 3/4. Tabela e gráficos
Preencher com `resultados.csv` gerado pelo `medir.sh`.

## 5. Pontos para a análise
- **Resultado diferente no último dígito:** com P threads, a soma vira P somas parciais
  combinadas no fim, outra ordem de arredondamento em ponto flutuante. A diferença fica
  na ordem de 10⁻¹³–10⁻¹⁴, bem dentro da tolerância 10⁻⁹. O próprio sequencial já
  difere do π real (`...589971` × `...589793`) pelo mesmo motivo (acúmulo de erro em 10⁹ somas).
- **Amdahl:** a fração sequencial (ler N, imprimir) é ~0, então o esperado é speedup
  quase linear até 6 núcleos físicos (Ryzen 5 5600X).
- **Se não for linear:** overhead de criar a região paralela (pequeno perto de ~1 s de
  trabalho), frequência de turbo caindo com mais núcleos ativos, ou outros processos rodando.
- **Comparar T(1) paralelo com o sequencial:** devem ser parecidos; se o paralelo com 1
  thread for mais lento, é o overhead do OpenMP.
- **Experimento extra (opcional):** trocar `reduction` por `#pragma omp atomic` e medir;
  mostra por que a sincronização por iteração destrói o desempenho.
