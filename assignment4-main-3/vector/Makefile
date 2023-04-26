CC=gcc
CFLAGS=-g -std=c11

ifeq ($(shell uname), Darwin)
	LEAKTEST ?= leaks --atExit --
else
	LEAKTEST ?= valgrind
endif

.PHONY: all valgrind clean test

all: vect_test

test: vect_test
	./vect_test

valgrind: vect_test
	$(LEAKTEST) ./vect_test

clean: 
	rm -rf *.o
	rm vect_test

vect_test: vect.o vect_test.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

