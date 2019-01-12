SHELL := /bin/bash

CC := gcc

CFLAGS = -Wall -Wextra -pedantic -Wno-unused-parameter \
		  -Wno-unused-but-set-parameter -Werror -std=c99 \
		  -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE $(INCLUDES)

INCLUDES := -iquote headers

LDLIBS := -lsqlite3

SUBDIRS := src

VPATH := $(shell echo `./getpaths.sh $(SUBDIRS)`)

OBJECTS := main.o hashtable.o s_linked_list.o sqlite3.o log.o

ifeq ($(MAKECMDGOALS),)
override CFLAGS += -O0  -g
else ifeq ($(MAKECMDGOALS),debug)
override CFLAGS += -O0  -g
else ifeq ($(MAKECMDGOALS),profile)
override CFLAGS += -O0 -pg
else ifeq ($(MAKECMDGOALS),production)
override CFLAGS += -O3
else ifeq ($(MAKECMDGOALS),cross)
CC = clang
CFLAGS = -Wall -Wextra -pedantic -Wno-unused-parameter -Werror -std=c99 -O0 -g -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE
endif

.PHONY: debug profile production cross clean

debug: $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o single-HTTP-debug

profile: $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o single-HTTP-profile

production: $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o single-HTTP

cross: $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o single-HTTP-cross

$(OBJECTS):

clean:
	$(RM) *.o
