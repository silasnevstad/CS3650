// Test malloc and free in order of allocation and reverse
// writing to all allocated locations with all 1s
// requesting random multiples of 8 bytes up to 2**22

#ifndef DEMO_TEST
#include <malloc.h>
extern int rand(void);
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

  for (i = 0; i < 100001; i++) {
    int *data = (int *) testmalloc(8 << (rand() % 20));
    int *data1 = (int *) testmalloc(8 << (rand() % 20));
    int *data2 = (int *) testmalloc(8 << (rand() % 20));
    int *data3 = (int *) testmalloc(8 << (rand() % 20));
    int *data4 = (int *) testmalloc(8 << (rand() % 20));
    int *data5 = (int *) testmalloc(8 << (rand() % 20));
    int *data6 = (int *) testmalloc(8 << (rand() % 20));
    int *data7 = (int *) testmalloc(8 << (rand() % 20));

    free(data);
    free(data1);
    free(data2);
    free(data3);
    free(data4);
    free(data5);
    free(data6);
    free(data7);

    // if ((i%100)==0) fprintf(stderr, "%d...", i);
  }
  fprintf(stderr, "malloc random size, free first to last malloc'd %d times.\n",
          i);

  for (i = 0; i < 100001; i++) {
    int *data = (int *) testmalloc(8 << (rand() % 20));
    int *data1 = (int *) testmalloc(8 << (rand() % 20));
    int *data2 = (int *) testmalloc(8 << (rand() % 20));
    int *data3 = (int *) testmalloc(8 << (rand() % 20));
    int *data4 = (int *) testmalloc(8 << (rand() % 20));
    int *data5 = (int *) testmalloc(8 << (rand() % 20));
    int *data6 = (int *) testmalloc(8 << (rand() % 20));
    int *data7 = (int *) testmalloc(8 << (rand() % 20));

    free(data7);
    free(data6);
    free(data5);
    free(data4);
    free(data3);
    free(data2);
    free(data1);
    free(data);

    // if ((i%100)==0) fprintf(stderr, "%d...", i);
  }
  fprintf(stderr, "malloc random size, free last to first malloc'd %d times.\n",
          i);

  return 0;
}
