#BIN = demo

CFLAGS += -Wall -Wextra -pedantic -Wno-unused-function -g
#-O2
CC=gcc
#SRC = main.c
#OBJ = $(SRC:.c=.o)
NUKLEAR=-I/home/user/Nuklear

LIBS=-lallegro -lallegro_main -lallegro_image -lallegro_font \
	-lallegro_ttf -lallegro_primitives -lm


#$(BIN):
#	@mkdir -p bin
#	rm -f bin/$(BIN) $(OBJS)
#	$(CC) ${NUKLEAR} $(SRC) $(CFLAGS) -o bin/$(BIN) $(LIBS)

all: 
	$(CC) ${NUKLEAR} main.c $(CFLAGS) -o sapper $(LIBS)

clean:
	rm sapper

