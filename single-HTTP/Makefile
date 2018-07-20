SHELL := /bin/bash

CC := gcc

CFLAGS := -Wall -Wextra -pedantic -Wno-unused-parameter \
		  -Wno-unused-but-set-parameter -Werror -std=c99 \
		  -D_POSIX_C_SOURCE=200809L

SUBDIRS := lib

OBJECTS := main.o hashtable.o s_linked_list.o

VPATH := $(shell echo `./getpaths.bash $(SUBDIRS)`)

ifeq ($(MAKECMDGOALS),)
override CFLAGS += -g
else ifeq ($(MAKECMDGOALS),debug)
override CFLAGS += -g
else ifeq ($(MAKECMDGOALS),profile)
override CFLAGS += -pg
else
override CFLAGS += -O3
endif

.PHONY: debug profile production clean

debug: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o single-HTTP-debug

profile: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o single-HTTP-profile

production: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o single-HTTP

$(OBJECTS):

clean:
	$(RM) *.o
