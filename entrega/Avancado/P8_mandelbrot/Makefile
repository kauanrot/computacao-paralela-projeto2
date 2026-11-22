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
	THREADS="1 2 4 6" ./medir.sh medicao.in
	./comparar_schedule.sh medicao.in

# versao rapida para testar/apresentar: entrada pequena, 2000 x 2000, Imax 2000 (a entrada.in tem 10^4, 10^4, 10^4)
rapido: all
	echo 2000 2000 2000 > pequeno.in
	./medir.sh pequeno.in
