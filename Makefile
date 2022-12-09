
CC = clang
CFLAGS = -g -Wall
OBJ = src/*.c
BIN = main

all: $(BIN)

main:  
	mkdir bin
	$(CC) $(CFLAGS) $(OBJ) -o bin/$(BIN)

clean:
	rm -rf bin
