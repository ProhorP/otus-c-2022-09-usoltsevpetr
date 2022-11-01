CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g

all: weather

weather: main.o
	$(CC) main.o -o weather -lcurl -ldl
main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o weather
