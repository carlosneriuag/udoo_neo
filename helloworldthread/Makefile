CC = gcc
CFLAGS = -Wall -Werror
OUTPUT = helloworldthread.out
SOURCES = helloworldthread.c
DEBUG = -g
LIBS = -lpthread

all:
	$(CC) $(CFLAGS) $(SOURCES) $(LIBS) -o $(OUTPUT)

debug:
	$(CC) $(CFLAGS) $(DEBUG)) $(SOURCES) $(LIBS) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)

fresh: clean all
