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

TARGETS = writer
OBJECTS = writer.o
SOURCES = writer.c
HEADERS = 

all: writer

writer: writer.o
	$(CC) -o writer writer.c

writer.o: writer.c
	$(CC) -c writer.c -o writer.o

clean:
	rm -f $(TARGETS) *.o
