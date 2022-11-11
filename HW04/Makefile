CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g

all: weather

weather: main.o parson.o
	$(CC) main.o parson.o -o weather -lcurl -ldl
main.o: main.c
	$(CC) $(CFLAGS) main.c 
	
parson.o: parson/parson.c	
	$(CC) $(CFLAGS) parson/parson.c

clean:
	rm -rf *.o weather
