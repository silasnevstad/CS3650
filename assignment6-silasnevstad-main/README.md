# Assignment 6: Concurrent Sorting

This is the starter code for [Assignment 6](https://course.ccs.neu.edu/cs3650f22/a06.html).

The [Makefile](Makefile) contains the following targets:

- `make all` - compile `msort` and `tmsort`
- `make msort` and `make tmsort` - compile the individual programs
- `make diff-N` - compile and run a diff test, comparing the results of `msort` and `tmsort` on a random input. `N` needs to be replaced by a positive integer. E.g., `make diff-100`.
- `make clean` - perform a minimal clean-up of the source tree

Note: This Makefile asks gcc to convert warnings into errors to help draw your attention to them.
