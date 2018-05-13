CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -Wno-unused-parameter \
		 -Wno-unused-but-set-parameter -Werror -std=c99 \
		 -D_POSIX_C_SOURCE=200809L

.PHONY: debug production clean

debug: main.o hashtable.o
	$(CC) $(CFLAGS) -g $< -o single-HTTP

production: main.o hashtable.o
	$(CC) $(CFLAGS) -O3 $< -o single-HTTP

main.o: main.c

hashtable.o: libraries/hashtable/hashtable.c

clean:
	$(RM) *.o
