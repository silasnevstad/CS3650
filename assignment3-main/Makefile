
all: compare array-max

compare: compare.c compare-main.s
	gcc -std=c99 -g compare.c compare-main.s -o compare

array-max: array-max.s array-max-main.c
	gcc -std=c99 -g array-max.s array-max-main.c -o array-max

clean:
	rm -f compare array-max
	rm -f *.o

a3.zip: compare.s compare-main.s array-max.s array-max-main.c compare-main.c 
	zip $@ $?

