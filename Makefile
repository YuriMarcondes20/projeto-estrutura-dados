CC     = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
SRC    = $(wildcard src/*.c)
BIN    = bin/sistema

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

run: all
	./$(BIN)

clean:
	rm -rf bin

.PHONY: all run clean
