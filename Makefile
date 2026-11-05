CC       := gcc
CFLAGS   := -Wall -Wextra -g -Iinclude -MMD -MP
LDFLAGS  :=
SANITIZE := -fsanitize=address,undefined

BIN := myshell
SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=build/%.o)
DEP := $(OBJ:.o=.d)

.PHONY: all asan valgrind test clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c -o $@ $<

build:
	mkdir -p build

# Sanitizer and valgrind builds must not mix objects, so both start clean.
asan:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) $(SANITIZE)" LDFLAGS="$(SANITIZE)"

valgrind:
	$(MAKE) clean
	$(MAKE)
	valgrind --leak-check=full --track-fds=yes ./$(BIN)

test: $(BIN)
	./tests/run.sh

clean:
	rm -rf build $(BIN)

-include $(DEP)
