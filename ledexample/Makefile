CC = gcc
CFLAGS = -Wall -Werror
SRCS = ledexample.c
OUT = ledexample.out

all:
	$(CC) $(SRCS) $(CFLAGS) -o $(OUT)

clean:
	rm -f $(OUT)

fresh:
	make clean
	make all

