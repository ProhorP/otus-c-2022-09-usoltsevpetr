CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g
GLIB_CONF=`pkg-config --cflags glib-2.0`
GLIB_LIB=`pkg-config --libs glib-2.0`
PCRE_LIB=`pcre-config --libs`

all: threads

threads: main.o
	$(CC) main.o $(GLIB_LIB) $(PCRE_LIB) -o threads

main.o: main.c
	$(CC) $(GLIB_CONF) $(CFLAGS) main.c 

clean:
	rm -rf *.o threads
