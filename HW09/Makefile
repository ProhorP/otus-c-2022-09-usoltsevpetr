CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g

all: daemon_stat

daemon_stat: main.o
	$(CC) main.o -o daemon_stat

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o daemon_stat
