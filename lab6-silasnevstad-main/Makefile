CC=gcc
CFLAGS=-g -std=c11
BINS=example1 example2 example3 example4 example5 example6 copy task

all: $(BINS)

$(BINS): %: %.o
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f $(BINS)
	rm -f *.o
	rm -f example2.txt example3.txt sorted_tail.txt
