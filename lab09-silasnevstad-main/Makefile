CC=gcc
CFLAGS=-g -std=c11 -pthread
BINS=relay_sem producer-consumer_condvar producer-consumer_sem

all: $(BINS)

$(BINS): %: %.c
	$(CC) $(CFLAGS) $^ -o $@

run_%: relay_%
	./$^

check_%: relay_%
	@echo Running $^ 5 times, checking its output...
	@((./$^ | diff correct_output.txt -) && \
		(./$^ | diff correct_output.txt -)	&& \
		(./$^ | diff correct_output.txt -)	&& \
		(./$^ | diff correct_output.txt -)	&& \
		(./$^ | diff correct_output.txt -)	&& \
		echo "Correct!") \
		|| echo "Incorrect"

clean:
	rm -f $(BINS)
	rm -f *.o
