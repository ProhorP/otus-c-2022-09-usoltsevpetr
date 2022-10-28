CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g

all: count_words

count_words: main.o
	$(CC) main.o -o count_words

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o count_words

