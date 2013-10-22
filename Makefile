PROG := ucp-assignment
LIBS :=
CFLAGS := -MMD -pedantic -Wall -Wextra
DBFLAGS := -g

# Should not have to change anything below here.

CC := gcc
OUTDIRS := obj

SRCFILES := $(wildcard src/*.c)

OBJFILES := $(patsubst src/%.c,obj/%.o,$(SRCFILES))
DEPFILES := $(patsubst src/%.c,obj/%.d,$(SRCFILES))

# Unit Testing
UTINC := -I$$HOME/.local/include
UTLIBS := -lcunit
UTLIBDIR := -L$$HOME/.local/lib
UTOUTDIRS := tests
UTSRCFILES := $(wildcard tests/src/*.c)
UTOBJFILES := $(patsubst tests/src/%.c,tests/obj/%.o,$(UTSRCFILES))
UTDEPFILES := $(patsubst tests/src/%.c,tests/obj/%.d,$(UTSRCFILES))

.PHONY: clean dirs

all: dirs $(PROG)

dirs:
	@mkdir -p $(OUTDIRS)

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
