CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -D_POSIX_C_SOURCE=200809 -g
GLIB_CONF=`pkg-config --cflags glib-2.0`
GLIB_LIB=`pkg-config --libs glib-2.0`

all: daemon_stat

daemon_stat: main.o
	$(CC) main.o $(GLIB_LIB) -o daemon_stat

main.o: main.c
	$(CC) $(GLIB_CONF) $(CFLAGS) main.c 

install:
	cp daemon_stat.service /etc/systemd/system/
	cp daemon_stat.ini /etc/
	cp daemon_stat /etc/init.d/

clean:
	rm -rf *.o daemon_stat
