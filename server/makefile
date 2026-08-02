ifdef CROSS_COMPILE
	ifeq ($(CROSS_COMPILE),aarch64-none-linux-gnu-)
		CC = aarch64-none-linux-gnu-gcc
	else
		CC = gcc
	endif
else
	CC = gcc
endif
CFLAGS = 

TARGETS = aesdsocket
OBJECTS = aesdsocket.o
SOURCES = aesdsocket.c
HEADERS = 

all: aesdsocket

default: aesdsocket

aesdsocket: aesdsocket.o
	$(CC) -o aesdsocket aesdsocket.c

aesdsocket.o: aesdsocket.c
	$(CC) -c aesdsocket.c -o aesdsocket.o

clean:
	rm -f $(TARGETS) *.o
