// A second test
#ifndef DEMO_TEST
#include <malloc.h>
#else
#include <stdlib.h>
#endif

#include <stdio.h>

int main() {

  // Constantly allocate and reallocate data.
  // Ideally your allocator can use the same block over
  // and over again.
  for (int i = 0; i < 100; i++) {
    int *data = (int *) malloc(1024);
    free(data);
  }

  return 0;
}
