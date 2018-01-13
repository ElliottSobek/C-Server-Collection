CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-unused-but-set-parameter -Werror -std=c99 -O0 -g

LDLIBS = -D_XOPEN_SOURCE=700

.PHONY: all clean

all: main

main: main.o
	$(CC) $(CFLAGS) $< -o single-HTTP

main.o: main.c
	$(CC) $(CFLAGS) $< $(LDLIBS) -c

clean:
	rm *.o single-HTTP
