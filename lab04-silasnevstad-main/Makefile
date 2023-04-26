CFLAGS := -g -std=c99
CC := gcc

ifeq ($(shell uname), Darwin)
	LEAKTEST ?= leaks --atExit --
else
	LEAKTEST ?= valgrind
endif

all: list_test

run: list_test
	./list_test

valgrind: list_test
	$(LEAKTEST) ./list_test

list_test: list_test.c linkedlist.c linkedlist.h
	$(CC) $(CFLAGS) -o list_test linkedlist.c list_test.c

.PHONY: clean valgrind run

clean:
	rm -f *.o list_test
	rm -rf *.dSYM

