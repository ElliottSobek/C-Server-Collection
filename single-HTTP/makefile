CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c99 -O0 -g

LDLIBS = -D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE

.PHONY: all clean

all: main

main: main.o
	$(CC) $(CFLAGS) $< -o single-HTTP

main.o: main.c
	$(CC) $(CFLAGS) $< $(LDLIBS) -c

clean:
	rm *.o single-HTTP
