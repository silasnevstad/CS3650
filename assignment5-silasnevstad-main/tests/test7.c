// Test interleaved mallocs and frees
// writing to all allocated locations with all 1s
// requesting random multiples of 8 bytes up to 2**22

#ifndef DEMO_TEST
#include <malloc.h>
extern int rand(void);
#else
#include <stdlib.h>
#endif

#include <stdio.h>
#include <stdlib.h>
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

  for (i = 0; i < 100001; i++) {
    int *data = (int *) testmalloc(8 << (rand() % 20));
    int *data1 = (int *) testmalloc(8 << (rand() % 20));
    free(data);
    free(data1);
    int *data2 = (int *) testmalloc(8 << (rand() % 20));
    int *data3 = (int *) testmalloc(8 << (rand() % 20));
    free(data2);
    free(data3);
    int *data4 = (int *) testmalloc(8 << (rand() % 20));
    free(data4);
    int *data5 = (int *) testmalloc(8 << (rand() % 20));
    int *data6 = (int *) testmalloc(8 << (rand() % 20));
    free(data5);
    free(data6);
    int *data7 = (int *) testmalloc(8 << (rand() % 20));
    free(data7);
  }
  fprintf(stderr, "malloc random size, interleaved frees %d times.\n", i - 1);

  return 0;
}
