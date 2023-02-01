CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g
INCLUDEDIR = -I`pg_config --includedir`
LIBDIR = -L`pg_config --libdir`

all: select

select: main.o
	$(CC) main.o -o select ${LIBDIR} -lpq 

main.o: main.c
	$(CC) $(CFLAGS) ${INCLUDEDIR} main.c 

clean:
	rm -rf *.o select
