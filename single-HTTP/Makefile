SHELL=/bin/bash

CC = gcc

CFLAGS = -Wall -Wextra -pedantic -Wno-unused-parameter \
		 -Wno-unused-but-set-parameter -Werror -std=c99 \
		 -D_POSIX_C_SOURCE=200809L

SUBDIRS = lib

.PHONY: debug profile production clean

debug: main.o minhashtable.o
	$(CC) $(CFLAGS) -g $< -o single-HTTP

profile: main.o minhashtable.o
	$(CC) $(CFLAGS) -g -pg $^ -o single-HTTP

production: main.o hashtable.o
	$(CC) $(CFLAGS) -O3 $^ -o single-HTTP

main.o: main.c

minhashtable.o: lib/minhashtable/minhashtable.c

hashtable.o: lib/hashtable/hashtable.c

clean:
	$(RM) *.o
