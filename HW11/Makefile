CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g

all: threads

threads: main.o
	$(CC) main.o -o threads

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o threads
