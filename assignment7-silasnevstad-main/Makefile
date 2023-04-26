CC=gcc
CFLAGS=-g -std=c11 -I.
BINS=mymalloc
TESTS=$(foreach n,1 2 3 4 5 6 7,tests/test$(n) )

define \n


endef

.PHONY: all clean test demo

all: mymalloc.o

help:
	@echo \
		"Available make targets: \n\
    make          Compile mymalloc.c to object file, mymalloc.o\n\
    make test     Compile and run tests in the tests directory with mymalloc.\n\
    make demo     Compile and run tests in the tests directory with standard malloc.\n\
    make clean    Clean up all generated files (executables and object files).\n\
    make help     Print available targets"

$(BINS): %: %.o
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(TESTS): %: %.o mymalloc.o

test: clean_tests $(TESTS)
	$(foreach t,$(TESTS),$(t)${\n})

demo: CFLAGS:=$(CFLAGS) -DDEMO_TEST

demo: test

clean: clean_tests
	rm -f $(BINS)
	rm -f *.o

clean_tests:
	rm -f tests/*.o
	rm -f $(TESTS)

