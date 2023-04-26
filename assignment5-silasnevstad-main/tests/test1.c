// Basic test

#ifndef DEMO_TEST
#include <malloc.h>
#else
#include <stdlib.h>
#endif

#include <stdio.h>

#define ARRAY_ELEMENTS 1024

int main() {

  // Allocate some data
  int *data = (int *) malloc(ARRAY_ELEMENTS * sizeof(int));

  // Do something with the data
  int i = 0;
  for (i = 0; i < ARRAY_ELEMENTS; i++) {
    data[i] = i;
  }

  // Free the data
  free(data);

  return 0;
}
