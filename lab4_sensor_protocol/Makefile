CC = gcc
CFLAGS = -Wall -Werror
SRCS = Shell/Shell.c Protocol/Protocol.c main.c
INCLUDES = -IShell -IProtocol
OUT = lab4_sensor_protocol.out
LIBS = -lpthread -lrt -lreadline
DEBUG = -g -D DEBUG

all:
	$(CC) $(SRCS) $(CFLAGS) $(INCLUDES) $(LIBS) -o $(OUT)

debug:
	$(CC) $(SRCS) $(CFLAGS) $(DEBUG) $(INCLUDES) $(LIBS) -o $(OUT)


clean:
	rm -f $(OUT)

fresh:
	make clean
	make all
