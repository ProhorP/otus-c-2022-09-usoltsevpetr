CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g

all: asm_to_c

asm_to_c: main.o
	$(CC) main.o -o asm_to_c

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o asm_to_c

