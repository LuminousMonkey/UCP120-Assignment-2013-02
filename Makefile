# Name of the executable to make.
PROG := ucp-assignment
LIBS = $(shell pkg-config --libs gtk+-2.0)

# Cflags for both unit tests and assignment
# Enable debugging for everything.
# Use MMD so GCC generates dep files for us.
COMMON_CFLAGS = -O0 -g -MMD -pedantic -Wall -Wextra
CFLAGS = $(COMMON_CFLAGS) $(shell pkg-config --cflags gtk+-2.0)

# Unit testing flags
UTCFLAGS = $(COMMON_CFLAGS) $(shell pkg-config --cflags cunit)
UTLIBS = $(shell pkg-config --libs cunit)

# Should not have to change anything below here.
CC := gcc
OUTDIRS := obj tests tests/obj

SRCFILES := $(wildcard src/*.c)

OBJFILES := $(patsubst src/%.c,obj/%.o,$(SRCFILES))
DEPFILES := $(patsubst src/%.c,obj/%.d,$(SRCFILES))

# Unit Testing
# Source files are just symlinked into tests/src dir.
UTOUTDIRS := tests
UTSRCFILES := $(wildcard tests/src/*.c)
UTOBJFILES := $(patsubst tests/src/%.c,tests/obj/%.o,$(UTSRCFILES))
UTDEPFILES := $(patsubst tests/src/%.c,tests/obj/%.d,$(UTSRCFILES))

# GCC does the work for us on determining file dependencies.
-include $(DEPFILES)
-include $(UTDEPFILES)

.PHONY: clean dirs

all: dirs $(PROG)

dirs:
	@mkdir -p $(OUTDIRS)

# Rule for creating the executable
$(PROG): $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Rule for making all *.c files in src to *.o files
obj/%.o: src/%.c
	$(CC) $(CFLAGS) -MF $(patsubst obj/%.o, obj/%.d,$@) -c $< -o $@

clean:
	rm -f $(OBJFILES) $(DEPFILES) $(PROG)
	rm -rf *.o *.d
	rm -f $(UTOBJFILES) $(UTDEPFILES)

tests: tests/unittests

# Rule for building CUnit
tests/unittests: $(UTOBJFILES)
	$(CC) $(UTCFLAGS) -o $@ $^ $(UTLIBS)

tests/obj/%.o: tests/src/%.c
	$(CC) $(UTCFLAGS) $(pathsub tests/obj/%.o, tests/obj/%.d,$@) -c $< -o $@
