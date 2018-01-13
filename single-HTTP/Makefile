CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -Wno-unused-parameter \
		 -Wno-unused-but-set-parameter -Werror -std=c99 -O0 -g \
		 -D_POSIX_C_SOURCE=200809L

.PHONY: all clean

all: main

main: main.o
	$(CC) $(CFLAGS) $< -o single-HTTP

main.o: main.c

clean:
	$(RM) *.o
