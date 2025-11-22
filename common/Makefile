# common/Makefile

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50
AR = ar
ARFLAGS = rcs

LIB = common.a
OBJS = pagedir.o index.o word.o

all: $(LIB)

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

pagedir.o: pagedir.c pagedir.h ../libcs50/webpage.h
	$(CC) $(CFLAGS) -c pagedir.c

index.o: index.c index.h ../libcs50/hashtable.h ../libcs50/counters.h ../libcs50/file.h
	$(CC) $(CFLAGS) -c index.c

word.o: word.c word.h
	$(CC) $(CFLAGS) -c word.c

# clean up
clean:
	rm -f *~ *.o $(LIB)