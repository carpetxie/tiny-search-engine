# Makefile 
# CS50 FA25 Final Project 'indexer' Module
# Author: Cindy Jiayi Liu, Group 1
# Date: 2025.11.15


CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../libcs50 -I../common
LIBS = ../common/common.a ../libcs50/libcs50.a
CC = gcc
MAKE = make

# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

all: indexer indextest

.PHONY: all test valgrind clean

################## indexer ###############
indexer: indexer.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@
# Dependencies
indexer.o: indexer.c ../common/pagedir.h ../common/index.h ../common/word.h ../libcs50/file.h ../libcs50/hashtable.h ../libcs50/webpage.h

test: indexer
	bash -v testing.sh

valgrind: indexer
	$(VALGRIND) ~/cs50-dev/shared/tse/indexcmp testingData/letters-1.index ~/cs50-dev/shared/tse/output/letters-1.index


################## indexertest ###############
indextest: indextest.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@
# Dependencies
indextest.o: indextest.c ../common/index.h ../libcs50/hashtable.h ../libcs50/webpage.h ../libcs50/counters.h 

indextesttest: indextest
	./indextest testingData/letters-2.index testingData/letters-2-indextest.index

indextestvalgrind: indextest
	$(VALGRIND) ~/cs50-dev/shared/tse/indexcmp testingData/letters-1.index testingData/letters-1-test.index


clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f indexer
	rm -f indextest
	rm -f core
