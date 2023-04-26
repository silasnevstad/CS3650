#ifndef DEMO_TEST
#include <malloc.h>
#else
#include <stdlib.h>
#endif

#include <stdio.h>

int main() {

  // Allocate several different uniquely sized blocks
  // Ideally your allocator will be able to re-use many
  // of these blocks.
  for (int i = 0; i < 100; i++) {
    int *data = (int *) malloc(8);
    int *data1 = (int *) malloc(16);
    int *data2 = (int *) malloc(32);
    int *data3 = (int *) malloc(64);
    int *data4 = (int *) malloc(128);

    free(data);
    free(data1);
    free(data2);
    free(data3);
    free(data4);
  }

  return 0;
}
