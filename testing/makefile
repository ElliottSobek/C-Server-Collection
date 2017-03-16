CC = gcc

CFLAGS = -Wall -Wextra -std=c99 -g -D_POSIX_C_SOURCE=200112L

.PHONY: all clean

all: testing

testing: testing.o

testing.o: testing.c

clean:
	rm *.o
