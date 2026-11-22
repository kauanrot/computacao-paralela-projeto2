CC     ?= gcc
CFLAGS ?= -O3 -march=native -Wall -Wextra -std=c11

all: sequencial paralelo

sequencial: sequencial.c
	$(CC) $(CFLAGS) -o $@ $<

paralelo: paralelo.c
	$(CC) $(CFLAGS) -fopenmp -o $@ $<

clean:
	rm -f sequencial paralelo

.PHONY: all clean rapido medir

# entrada de referencia do caderno (N=5000, G=500, seed 42), caso nao exista
entrada.in:
	python3 gen_life.py > $@

# medicao para o relatorio: P = 1, 2, 4, 6 (o caderno pede o 6 mesmo com menos nucleos)
medir: all
	THREADS="1 2 4 6" ./medir.sh

# versao rapida para testar/apresentar: entrada pequena, 1000 x 1000, 100 geracoes (a entrada.in tem 5000 x 5000, 500)
rapido: all
	python3 gen_life.py 1000 100 > pequeno.in
	./medir.sh pequeno.in
