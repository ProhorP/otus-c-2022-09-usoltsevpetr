CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -g
LIB_ALLEGRO=-lallegro -lallegro_ttf -lallegro_font -lallegro_image

all: sapper

sapper: main.o
	$(CC) main.o -o sapper ${LIB_ALLEGRO}

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o sapper
