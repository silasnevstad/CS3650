// Test malloc and free in order of allocation and reverse
// writing to all allocated locations with all 1s
// requesting ordered multiples of 8 bytes up to 2**10

#ifndef DEMO_TEST
#include <malloc.h>
#else
#include <stdlib.h>
#endif

#include <stdio.h>
#include <string.h>

int allones = ~0; // allones for int

int *testmalloc(int size) {
  int *data = (int *) malloc(size);
  if (data != NULL) {
    // void *memset(void *s, int c, size_t n);
    memset((void *) data, allones, size);
  }
  return data;
}

int main() {
  int i;

  for (i = 0; i < 1000; i++) {
    int *data = (int *) testmalloc(8);
    int *data1 = (int *) testmalloc(16);
    int *data2 = (int *) testmalloc(32);
    int *data3 = (int *) testmalloc(64);
    int *data4 = (int *) testmalloc(128);
    int *data5 = (int *) testmalloc(256);
    int *data6 = (int *) testmalloc(512);
    int *data7 = (int *) testmalloc(1024);

    free(data);
    free(data1);
    free(data2);
    free(data3);
    free(data4);
    free(data5);
    free(data6);
    free(data7);
  }
  fprintf(stderr, "malloc small to large, free small to large %d times.\n", i);

  for (i = 0; i < 1000; i++) {
    int *data = (int *) testmalloc(8);
    int *data1 = (int *) testmalloc(16);
    int *data2 = (int *) testmalloc(32);
    int *data3 = (int *) testmalloc(64);
    int *data4 = (int *) testmalloc(128);
    int *data5 = (int *) testmalloc(256);
    int *data6 = (int *) testmalloc(512);
    int *data7 = (int *) testmalloc(1024);

    free(data7);
    free(data6);
    free(data5);
    free(data4);
    free(data3);
    free(data2);
    free(data1);
    free(data);
  }
  fprintf(stderr, "malloc small to large, free large to small %d times.\n", i);

  return 0;
}
