.POSIX:

.SUFFIXES:
.SUFFIXES: .c .h .o

.PHONY: debug profile cross production dist install TAGS test print tar clean clobber

SHELL := /bin/bash

CC := gcc

CFLAGS := -Wall -Wextra -pedantic -Wno-unused-parameter \
		 -Wno-unused-but-set-parameter -std=c99 \
		 -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE -iquote headers

LDLIBS := -lsqlite3

SRCDIRS := src

OBJDIR := obj

DEPDIR := headers

VPATH := $(shell echo `./getpaths.sh c $(SRCDIRS)`)

SRC := $(shell echo `find -O3 $(SRCDIRS) -type f -name "*.c" -printf "%f "`)

OBJECTS = $(addprefix $(OBJDIR)/,$(SRC:.c=.o))

EXECUTABLES := single-HTTP-debug single-HTTP-profile single-HTTP-cross single-HTTP

DISTFILENAME := single-HTTP.tar.bz2

TARFILENAME := single-HTTP.tar

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

debug: single-HTTP-debug

profile: single-HTTP-profile

cross: single-HTTP-cross

production: single-HTTP

single-HTTP-debug: $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

single-HTTP-profile: $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

single-HTTP-cross: $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

single-HTTP: $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $? -o $@

dist:
	tar -cj --posix -f $(DISTFILENAME) $(SRCDIRS) $(DEPDIR)

install:
	@echo "Not Implemented"

TAGS:
	@echo "Not Implemented"

test:
	@echo "Not Implemented"

print: %.c
	@echo $?

tar:
	tar -c --posix -f $(TARFILENAME) $(SRCDIRS) $(DEPDIR)

clean:
	$(RM) $(OBJDIR)/*.o

clobber:
	$(RM) $(OBJDIR)/*.o $(EXECUTABLES) $(DISTFILENAME) $(TARFILENAME)
