# crawler/Makefile

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../common

PROG = crawler
OBJS = crawler.o
LIBS = ../common/pagedir.o \
       ../libcs50/bag.o \
       ../libcs50/hashtable.o \
       ../libcs50/webpage.o \
       ../libcs50/mem.o \
       ../libcs50/set.o \
       ../libcs50/hash.o \
       ../libcs50/file.o

.PHONY: all clean test

# ------------ default target ------------
all: $(PROG)

# ------------ link the crawler program ------------
$(PROG): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# ------------ compile crawler.o ------------
crawler.o: crawler.c ../common/pagedir.h \
                     ../libcs50/webpage.h \
                     ../libcs50/bag.h \
                     ../libcs50/hashtable.h
	$(CC) $(CFLAGS) -c crawler.c

# ------------ build common and libcs50 .o files ------------
../common/pagedir.o: ../common/pagedir.c ../common/pagedir.h ../libcs50/webpage.h
	$(CC) $(CFLAGS) -c -o $@ $<

../libcs50/bag.o: ../libcs50/bag.c ../libcs50/bag.h
	$(CC) $(CFLAGS) -c -o $@ $<

../libcs50/hashtable.o: ../libcs50/hashtable.c ../libcs50/hashtable.h ../libcs50/set.h ../libcs50/hash.h
	$(CC) $(CFLAGS) -c -o $@ $<

../libcs50/webpage.o: ../libcs50/webpage.c ../libcs50/webpage.h ../libcs50/file.h
	$(CC) $(CFLAGS) -c -o $@ $<

../libcs50/mem.o: ../libcs50/mem.c ../libcs50/mem.h
	$(CC) $(CFLAGS) -c -o $@ $<

../libcs50/set.o: ../libcs50/set.c ../libcs50/set.h
	$(CC) $(CFLAGS) -c -o $@ $<

../libcs50/hash.o: ../libcs50/hash.c ../libcs50/hash.h
	$(CC) $(CFLAGS) -c -o $@ $<

../libcs50/file.o: ../libcs50/file.c ../libcs50/file.h
	$(CC) $(CFLAGS) -c -o $@ $<

# ------------ clean ------------
clean:
	rm -f *~ *.o $(PROG)

# ------------ test (runs testing.sh -> testing.out) ------------
test: $(PROG)
	bash -v testing.sh > testing.out 2>&1
	@echo "Testing complete; results saved to testing.out"

# ------------ valgrind ------------
valgrind: $(PROG)
	mkdir -p ../data/valgrind-letters-0
	$(VALGRIND) ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/valgrind-letters-0 0