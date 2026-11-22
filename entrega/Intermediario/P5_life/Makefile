CC     ?= gcc
CFLAGS ?= -O3 -march=native -Wall -Wextra -std=c11

all: sequencial paralelo

sequencial: sequencial.c
	$(CC) $(CFLAGS) -o $@ $<

paralelo: paralelo.c
	$(CC) $(CFLAGS) -fopenmp -o $@ $<

clean:
	rm -f sequencial paralelo

.PHONY: all clean

# entrada de referencia do caderno (N=5000, G=500, seed 42), caso nao exista
entrada.in:
	python3 gen_life.py > $@
