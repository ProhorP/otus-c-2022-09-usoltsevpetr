CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g

all: start

start: main.o
	$(CC) main.o -o start

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o start
