CC = gcc
CFLAGS = -g -Wall -Werror -std=c99

deps: deps.o

clean:
	rm -f deps *.o
