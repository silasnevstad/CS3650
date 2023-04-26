CC=gcc
CFLAGS=-g -std=c11

ifeq ($(shell uname), Darwin)
	LEAKTEST ?= leaks --atExit --
else
	LEAKTEST ?= valgrind
endif

.PHONY: all valgrind clean test

all: queue_test

test: queue_test
	./queue_test

valgrind: queue_test
	$(LEAKTEST) ./queue_test

clean: 
	rm -rf *.o
	rm queue_test

queue_test: queue.o queue_test.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

