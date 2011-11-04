CC = gcc
CFLAGS = -g -std=c99
LD = gcc
LDFLAGS = 

SRC = src
BUILD = build
OBJ = $(BUILD)/obj
GENSRC = $(BUILD)/gensrc
GENOBJ = $(BUILD)/genobj
BIN = bin


OBJS = \
  $(OBJ)/common.o \
  $(OBJ)/main.o \
  $(OBJ)/parse.o \
  $(OBJ)/stringset.o \
  $(GENOBJ)/conditions.tab.o \
  $(GENOBJ)/conditions.yy.o


.PHONY: default
default: dirs $(BIN)/deps

$(BIN)/deps: $(OBJS)
	$(LD) -o $@ $(LDFLAGS) $^

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -o $@ -c $(CFLAGS) $<

$(GENOBJ)/%.o: $(GENSRC)/%.c
	$(CC) -o $@ -c -I$(SRC) $(CFLAGS) $<

$(GENSRC)/conditions.tab.h $(GENSRC)/conditions.tab.c: $(SRC)/conditions.y
	bison --defines -o $(GENSRC)/conditions.tab.c $<

$(GENSRC)/conditions.yy.c: $(SRC)/conditions.l $(GENSRC)/conditions.tab.h
	flex -o $@ $<


.PHONY: dirs
dirs:
	@mkdir -p $(BUILD)
	@mkdir -p $(OBJ)
	@mkdir -p $(GENSRC)
	@mkdir -p $(GENOBJ)
	@mkdir -p $(BIN)

clean:
	rm -rf $(BIN) $(BUILD)
