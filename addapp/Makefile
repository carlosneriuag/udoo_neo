CC = gcc
CFLAGS = -Wall -Werror
OUTPUT = addapp.out
SOURCES = addapp.c
DEBUG = -g

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT)

debug:
	$(CC) $(CFLAGS) $(DEBUG) $(SOURCES) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)

fresh: clean all
