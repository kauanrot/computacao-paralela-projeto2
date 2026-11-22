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

# medicao para o relatorio: P = 1, 2, 4, 6 (o caderno pede o 6 mesmo com menos nucleos)
medir: all
	THREADS="1 2 4 6" ./medir.sh

# versao rapida para testar/apresentar: entrada pequena, N = 2*10^8 (a entrada.in tem 10^9)
rapido: all
	echo 200000000 > pequeno.in
	./medir.sh pequeno.in
