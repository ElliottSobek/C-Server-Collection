CC = gcc

CFLAGS = -Wall -Wextra -std=c99 -g

.PHONY: all memcheck clean

all: main

main: main.o
	$(CC) $(CFLAGS) main.o -o single-HTTP

main.o: main.c
	$(CC) $(CFLAGS) main.c -D_POSIX_C_SOURCE=200112L -c

clean:
	rm *.o single-HTTP

memcheck:
	valgrind --leak-check=full -v ./single-HTTP
