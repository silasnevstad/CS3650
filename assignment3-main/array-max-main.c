
#include <stdio.h>
#include <stdlib.h>

extern unsigned long array_max(unsigned long, unsigned long *);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Missing argument.\n\nUsage: %s <n1> <n2> ...\n", argv[0]);
    return 1;
  }

  unsigned long nums[argc - 1];

  for (int i = 1; i < argc ; ++i) {
    nums[i - 1] = atol(argv[i]);
  }

  printf("%lu\n", array_max(argc - 1, nums));

  return 0;

}
