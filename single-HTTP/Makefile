.POSIX:

.SUFFIXES:
.SUFFIXES: .c .h .o

.PHONY: debug profile cross production all install print tar shar dist TAGS test clean mostlyclean clobber

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

all:
	@echo -e "Action not applicable for project.\nRefer to debug, profile, cross, or production targets."

install:
	@echo "Not Implemented"

print:
	@./changedSources.sh src c obj o

tar:
	tar -cSW --posix --restrict -f $(TARFILENAME) $(SRCDIRS) $(DEPDIR)

shar:
	@{ find -O3 src/ -type f -name "*.c" & find -O3 headers/ -type f -name "*.h"; } | shar -qV > single-HTTP.shar

dist:
	@tar -cSj --no-auto-compress --posix --restrict -f $(DISTFILENAME) $(SRCDIRS) $(DEPDIR)

TAGS:
	@echo "Action not applicable for project"

test:
	@echo "Not Implemented"

clean:
	$(RM) $(OBJDIR)/*.o

mostlyclean:
	@echo "Action not applicable for project"

clobber:
	$(RM) $(OBJDIR)/*.o $(EXECUTABLES) $(DISTFILENAME) $(TARFILENAME)
