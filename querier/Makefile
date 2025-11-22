# Makefile for querier

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../common
LLIBS = ../libcs50/libcs50.a ../common/common.a

PROG = querier
OBJS = querier.o

.PHONY: all clean test

all: $(PROG)

$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

querier.o: querier.c

test: $(PROG) testing.sh
	bash -v testing.sh &> testing.out

clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	rm -f core

