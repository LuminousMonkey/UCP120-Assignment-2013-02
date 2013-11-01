PROG := ucp-assignment
LIBS :=
CFLAGS := -MMD -pedantic -Wall -Wextra
DBFLAGS := -g

# Should not have to change anything below here.

CC := gcc
OUTDIRS := obj tests/obj

SRCFILES := $(wildcard src/*.c)

OBJFILES := $(patsubst src/%.c,obj/%.o,$(SRCFILES))
DEPFILES := $(patsubst src/%.c,obj/%.d,$(SRCFILES))

# Unit Testing
# Make sure it can find the CUnit includes and library
UTCFLAGS := -g
UTLIBS := $(shell pkg-config --cflags --libs cunit)
UTOUTDIRS := tests

UTSRCFILES := $(wildcard tests/src/*.c)

UTOBJFILES := $(patsubst tests/src/%.c,tests/obj/%.o,$(UTSRCFILES))
UTDEPFILES := $(patsubst tests/src/%.c,tests/obj/%.d,$(UTSRCFILES))

.PHONY: clean dirs

all: dirs $(PROG)

dirs:
	@mkdir -p $(OUTDIRS)

tests: tests/unittests

tests/unittests: $(UTOBJFILES)
	$(CC) $(STD) $(UTCFLAGS) -o $@ $^ $(UTLIBS)

tests/obj/%.o: tests/src/%.c
	$(CC) $(DBFLAGS) $(STD) $(CFLAGS) $(pathsub tests/obj/%.o, tests/obj/%.d,$@) -c $< -o $@

$(PROG): $(OBJFILES)
ifeq ($(DEBUG),0)
	$(CC) $(STD) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	strip $@
	@echo "---- Created release binary ----"
else
	$(CC) $(STD) $(DBFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	@echo "---- Created debug binary ----"
endif

-include $(DEPFILES)
-include $(UTDEPFILES)

obj/%.o: src/%.c
ifeq ($(DEBUG),0)
	$(CC) $(RELFLAGS) $(STD) $(CFLAGS) -MF $(patsubst obj/%.o, obj/%.d,$@) -c $< -o $@
else
	$(CC) $(DBFLAGS) $(STD) $(CFLAGS) -MF $(patsubst obj/%.o, obj/%.d,$@) -c $< -o $@
endif

clean:
	rm -f $(OBJFILES) $(DEPFILES) $(PROG)
	rm -rf *.o *.d
	rm -f $(UTOBJFILES) $(UTDEPFILES)
