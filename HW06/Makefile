CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g

all: test

test: main.o liblog.a
	$(CC) main.o -L. -llog -o test 
main.o: main.c
	$(CC) $(CFLAGS) main.c 
	
liblog.a: liblog.o
	ar cr liblog.a liblog.o
liblog.o: liblog.c	
	$(CC) $(CFLAGS) liblog.c

clean:
	rm -rf *.o test liblog.a
