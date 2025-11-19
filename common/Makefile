# common/Makefile

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50

OBJS = pagedir.o
LIBS =

# Default rule: build all object files
all: $(OBJS)

# pagedir object file
pagedir.o: pagedir.c pagedir.h ../libcs50/webpage.h
	$(CC) $(CFLAGS) -c pagedir.c

# clean up
clean:
	rm -f *~ *.o