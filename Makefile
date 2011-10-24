CC = gcc
CFLAGS = -g -Wall -Werror -std=c99

OBJS = \
  common.o \
  main.o \
  parse.o

deps: $(OBJS)
	$(CC) -o $@ $(CFLAGS) $^

clean:
	rm -f deps *.o
