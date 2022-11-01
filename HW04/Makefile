CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g

all: weater

weater: main.o
	$(CC) main.o -o weater

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o weater
