CC = gcc
CFLAGS = -Wall -Werror
OUTPUT = helloworld.out
SOURCES = helloworld.c
DEBUG = -g

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT)

debug:
	$(CC) $(CFLAGS) $(DEBUG) $(SOURCES) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)

fresh: clean all
