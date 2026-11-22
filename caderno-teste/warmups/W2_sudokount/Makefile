CC      ?= gcc
CFLAGS  ?= -O3 -march=native -Wall -Wextra -std=c11
TARGET  := $(notdir $(CURDIR))
all: $(TARGET)
$(TARGET): $(wildcard *.c)
	$(CC) $(CFLAGS) -o $@ $<
clean:
	rm -f $(TARGET)
.PHONY: all clean
