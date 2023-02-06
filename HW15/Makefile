CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g
OG=-lGL -lGLU -lglut

all: rotate_cube

rotate_cube: main.o
	$(CC) main.o -o rotate_cube ${OG}

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o rotate_cube
