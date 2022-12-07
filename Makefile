
CC=clang
CFLAGS=-g -Wall
OBJ=minicron.c
BIN=minicron

all:$(BIN)

minicron: $(OBJ) 
	$(CC) $(CFLAGS) $(OBJ) -o minicron

clean:
	rm -rf minicron
