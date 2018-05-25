SHELL := /bin/bash

CC := gcc

CFLAGS := -Wall -Wextra -pedantic -Wno-unused-parameter \
		  -Wno-unused-but-set-parameter -Werror -std=c99 \
		  -D_POSIX_C_SOURCE=200809L

SUBDIRS := lib

OBJECTS := main.o hashtable.o

VPATH := $(shell echo `./getpaths.bash $(SUBDIRS)`)

.PHONY: debug profile production clean

debug: $(OBJECTS)
	$(CC) $(CFLAGS) -g $^ -o single-HTTP-debug

profile: $(OBJECTS)
	$(CC) $(CFLAGS) -pg $^ -o single-HTTP-profile

production: $(OBJECTS)
	$(CC) $(CFLAGS) -O3 $^ -o single-HTTP

$(OBJECTS):

clean:
	$(RM) *.o
