CC = gcc
CFLAGS = -Wall -Werror
OUTPUT = accelerometer.out
DEBUG = -g
SOURCES = main.c I2cWrapper.c

all:
	$(CC) $(SOURCES) $(CFLAGS) -o $(OUTPUT)

debug:
	$(CC) $(SOURCES) $(CFLAGS) $(DEBUG) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)

fresh:
	make clean
	make all
