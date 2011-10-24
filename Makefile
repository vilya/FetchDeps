CC = gcc
CFLAGS = -g -Wall -Werror -std=c99

OBJS = \
  common.o \
  deps.o \
  parse.o

deps: $(OBJS)

clean:
	rm -f deps *.o
