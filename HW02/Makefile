CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g

all: convert

convert: main.o
	$(CC) main.o -o convert

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o convert
