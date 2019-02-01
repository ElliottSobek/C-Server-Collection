.POSIX:

.SUFFIXES:
.SUFFIXES: .c .h .o

.PHONY: clean

SHELL := /bin/bash

CC := gcc

CFLAGS := -Wall -Wextra -pedantic -Wno-unused-parameter \
		 -Wno-unused-but-set-parameter -std=c99 \
		 -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE -iquote headers

LDLIBS := -lsqlite3

SUBDIRS := src

OBJDIR := obj/

VPATH := $(shell echo `./getpaths.sh c $(SUBDIRS)`)

SRC := $(shell echo `find -O3 $(SUBDIRS) -type f -name "*.c" -printf "%f "`)

OBJECTS = $(addprefix $(OBJDIR),$(SRC:.c=.o))

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

single-HTTP-debug: $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

single-HTTP-profile: $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

single-HTTP-cross: $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

single-HTTP: $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

$(OBJDIR)%.o: %.c
	$(CC) $(CFLAGS) -c $? -o $@

clean:
	$(RM) $(OBJDIR)*.o
