CC = gcc
CFLAGS = -Wall -Werror
OUTPUT = filemanagement.out
SOURCES = filemanagement.c
DEBUG = -g

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT)

debug:
	$(CC) $(CFLAGS) $(DEBUG) $(SOURCES) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)

fresh: clean all
