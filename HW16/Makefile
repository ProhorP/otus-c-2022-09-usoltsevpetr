CFLAGS += -Wall -Wextra -pedantic -Wno-unused-function -g
CC=gcc
NUKLEAR=-I/home/user/Nuklear

LIBS=-lallegro -lallegro_main -lallegro_image -lallegro_font \
	-lallegro_ttf -lallegro_primitives -lm

all: 
	$(CC) ${NUKLEAR} main.c $(CFLAGS) -o sapper $(LIBS)

clean:
	rm sapper

