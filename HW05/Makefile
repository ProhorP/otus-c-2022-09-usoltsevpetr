CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g

all: my_player

my_player: main.o
	$(CC) main.o -o my_player

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o my_player
