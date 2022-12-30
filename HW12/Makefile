CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g

all: telehack

telehack: main.o
	$(CC) main.o -o telehack

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o telehack
