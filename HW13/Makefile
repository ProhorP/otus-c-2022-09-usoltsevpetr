CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g

all: http_server

http_server: main.o
	$(CC) main.o -o http_server

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o http_server
