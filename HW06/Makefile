CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g

all: test

test: main.o liblog.o
	$(CC) main.o liblog.o -o test 
main.o: main.c
	$(CC) $(CFLAGS) main.c 
	
liblog.o: liblog.c	
	$(CC) $(CFLAGS) liblog.c

clean:
	rm -rf *.o test
