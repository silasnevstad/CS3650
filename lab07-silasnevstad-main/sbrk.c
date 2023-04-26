// sbrk.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void *mymalloc(size_t size) {
  void *ptr = sbrk(0);
  void *request = sbrk(size);
  if (request == (void *) -1) {
    return NULL;
  }

  return ptr;
}

void setupProgram() {
  // Memory leak! But why?
  mymalloc(2);
  mymalloc(4);
  mymalloc(8);
  mymalloc(16);
  mymalloc(32);
  mymalloc(64);
  mymalloc(128);
  mymalloc(256);
}

int main(int argc, char **argv) {

  setupProgram();

  return 0;
}
