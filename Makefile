CC = gcc
CFLAGS = -g
LD = gcc
LDFLAGS = -lcurl

SRC = src
BUILD = build
OBJ = $(BUILD)/obj
GENSRC = $(BUILD)/gensrc
GENOBJ = $(BUILD)/genobj
BIN = bin


OBJS = \
  $(GENOBJ)/conditions.tab.o \
  $(GENOBJ)/conditions.yy.o \
  $(OBJ)/download.o \
  $(OBJ)/filesys.o \
  $(OBJ)/main.o \
  $(OBJ)/parse.o \
  $(OBJ)/stringset.o \
  $(OBJ)/varmap.o


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
