CC = gcc
CFLAGS = -Wall -Werror
SRCS = main.c Shell.c
OUT = shell_test.out
LIBS = -lpthread -lreadline
all:
	$(CC) $(SRCS) $(CFLAGS) $(LIBS) -o $(OUT)

debug:
	$(CC) $(SRCS) $(CFLAGS) -g -D DEBUG $(LIBS) -o $(OUT)
    
clean:
	rm -f $(OUT)

fresh:
	make clean
	make all
