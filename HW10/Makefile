CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g

all: crc32

crc32: main.o
	$(CC) main.o -o crc32

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o crc32
