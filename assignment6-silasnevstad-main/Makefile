CC=gcc
CFLAGS=-g -std=gnu11 -Werror

msort_OBJS=$(patsubst %.c,%.o,$(filter-out tmsort.c,$(wildcard *.c)))
tmsort_OBJS=$(patsubst %.c,%.o,$(filter-out msort.c,$(wildcard *.c)))

ifeq ($(shell uname), Darwin)
	LEAKTEST ?= leaks --atExit --
else
	LEAKTEST ?= valgrind --leak-test=full
endif

.PHONY: all valgrind clean test

all: msort tmsort

valgrind: valgrind-msort valgrind-tmsort

valgrind-%: %
	$(LEAKTEST) ./$* $(ARGS)

clean: 
	rm -rf *.o
	rm -f msort tmsort

diff-%: msort tmsort
	$(eval TMP := $(shell mktemp -d))
	$(info == Running diff test in $(TMP) ==)
	@cd $(TMP) && shuf -i1-$* > input.txt
	@cd $(TMP) && $(CURDIR)/msort $* < input.txt > msort.txt
	@cd $(TMP) && $(CURDIR)/tmsort $* < input.txt > tmsort.txt
	@echo
	@echo "== Files msort.txt and tmsort.txt should be the same. =="

	@cd $(TMP) && diff -sq msort.txt tmsort.txt
	@rm -rf $(TMP)

msort: $(msort_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

tmsort: $(tmsort_OBJS)
	$(CC) -pthread $(CFLAGS) -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

