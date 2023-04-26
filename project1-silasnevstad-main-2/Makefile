CC=gcc
CFLAGS=-g -std=c11

TOKENIZE_OBJS=$(patsubst %.c,%.o,$(filter-out shell.c,$(wildcard *.c)))
SHELL_OBJS=$(patsubst %.c,%.o,$(filter-out tokenize.c,$(wildcard *.c)))

ifeq ($(shell uname), Darwin)
	LEAKTEST ?= leaks --atExit --
else
	LEAKTEST ?= valgrind --leak-test=full
endif

.PHONY: all valgrind clean test

all: shell tokenize

valgrind: shell tokenize
	$(LEAKTEST) ./tokenize
	$(LEAKTEST) ./shell

tokenize-tests shell-tests : %-tests: %
	bash tests/$@

test: tokenize-tests shell-tests 

clean: 
	rm -rf *.o
	rm -f shell tokenize

shell: $(SHELL_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

tokenize: $(TOKENIZE_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

