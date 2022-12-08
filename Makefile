
CC = clang
CFLAGS = -g -Wall
OBJ = src/minicron.c
BIN = minicron

all: $(BIN)

minicron:  
	mkdir bin
	$(CC) $(CFLAGS) $(OBJ) -o bin/$(BIN)

clean:
	rm -rf bin
