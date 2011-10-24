CC = gcc
CFLAGS = -g -Wall -Werror -std=c99
LD = gcc
LDFLAGS = 

SRC = src
OBJ = build/obj
BIN = bin

OBJS = \
  $(OBJ)/common.o \
  $(OBJ)/main.o \
  $(OBJ)/parse.o


.PHONY: default
default: dirs $(BIN)/deps

$(BIN)/deps: $(OBJS)
	$(LD) -o $@ $(LDFLAGS) $^

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -o $@ -c $(CFLAGS) $<

.PHONY: dirs
dirs:
	@mkdir -p $(OBJ)
	@mkdir -p $(BIN)

clean:
	rm -rf $(BIN) build
