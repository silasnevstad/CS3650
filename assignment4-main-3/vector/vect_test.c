#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "vect.h"

#ifndef _TESTINFRA_H
#include <assert.h>
#define DO_TEST(x) (assert(x))
#define TEST_BEGIN(description, ...) \
  fprintf(stderr, "TEST: " description " ... ", ##__VA_ARGS__)
#define TEST_END() fputs("OK\n", stderr)
#endif

void big_test(vect_t *v, unsigned int count) {
  TEST_BEGIN("Adding %d items", count);
  char buf[128];
  for (int i = 0; i < count; i++) {
    sprintf(buf, "Item no. %d", i);
    vect_add(v, buf);
  }
  DO_TEST(vect_size(v) == count);
  TEST_END();

  TEST_BEGIN("Ensuring %d items added correctly", count);
  for (int i = 0; i < count; i++) {
    sprintf(buf, "Item no. %d", i);
    DO_TEST(strcmp(vect_get(v, i), buf) == 0);
  }
  TEST_END();

  TEST_BEGIN("Modifying all %d items", count);
  for (int i = 0; i < count; i++) {
    sprintf(buf, "%s, %d", vect_get(v, i), i * i);
    vect_set(v, i, buf);
  }

  for (int i = 0; i < count; i++) {
    sprintf(buf, "Item no. %d, %d", i, i * i);
    DO_TEST(strcmp(vect_get(v, i), buf) == 0);
  }
  TEST_END();
}

int main(int argc, char **argv) {
  int bigger = 0;

  if (argc > 1 && strcmp(argv[1], "--bigger") == 0) 
    bigger = 1;

  TEST_BEGIN("Creating new empty vector");
  vect_t *v1 = vect_new();
  DO_TEST(v1);
  DO_TEST(vect_current_capacity(v1) == 2);
  DO_TEST(vect_size(v1) == 0);
  TEST_END();


  TEST_BEGIN("Adding a single item");
  vect_add(v1, "hello");
  DO_TEST(vect_current_capacity(v1) == 2);
  DO_TEST(vect_size(v1) == 1);
  DO_TEST(strcmp(vect_get(v1, 0), "hello") == 0);
  TEST_END();

  TEST_BEGIN("Adding a second item");
  vect_add(v1, "world");
  DO_TEST(vect_current_capacity(v1) == 2);
  DO_TEST(vect_size(v1) == 2);
  DO_TEST(strcmp(vect_get(v1, 0), "hello") == 0);
  DO_TEST(strcmp(vect_get(v1, 1), "world") == 0);
  TEST_END();

  TEST_BEGIN("Adding a third item");
  vect_add(v1, "CS3650");
  DO_TEST(vect_current_capacity(v1) == 4);
  DO_TEST(vect_size(v1) == 3);
  DO_TEST(strcmp(vect_get(v1, 0), "hello") == 0);
  DO_TEST(strcmp(vect_get(v1, 1), "world") == 0);
  DO_TEST(strcmp(vect_get(v1, 2), "CS3650") == 0);
  TEST_END();

  TEST_BEGIN("Modifying an item");
  vect_set(v1, 1, "Foo");
  DO_TEST(vect_current_capacity(v1) == 4);
  DO_TEST(vect_size(v1) == 3);
  DO_TEST(strcmp(vect_get(v1, 0), "hello") == 0);
  DO_TEST(strcmp(vect_get(v1, 1), "Foo") == 0);
  DO_TEST(strcmp(vect_get(v1, 2), "CS3650") == 0);
  TEST_END();

  TEST_BEGIN("Removing from the end of the vector");
  vect_remove_last(v1);
  DO_TEST(vect_current_capacity(v1) == 4);
  DO_TEST(vect_size(v1) == 2);
  DO_TEST(strcmp(vect_get(v1, 0), "hello") == 0);
  DO_TEST(strcmp(vect_get(v1, 1), "Foo") == 0);
  TEST_END();

  TEST_BEGIN("Checking behavior of vect_get and vect_get_copy");
  const char *tmp1 = vect_get(v1, 0);
  const char *tmp2 = vect_get(v1, 0);
  DO_TEST(strcmp(tmp1, tmp2) == 0);
  DO_TEST(tmp1 == tmp2);

  char *copy1 = vect_get_copy(v1, 1);
  char *copy2 = vect_get_copy(v1, 1);
  DO_TEST(strcmp(copy1, copy2) == 0);
  DO_TEST(copy1 != copy2);
  DO_TEST(copy1 != vect_get(v1, 1));
  free(copy1);
  free(copy2);
  TEST_END();

  TEST_BEGIN("Deleting vector");
  vect_delete(v1);
  TEST_END();

  TEST_BEGIN("Creating another vector");
  v1 = vect_new();
  DO_TEST(v1);
  TEST_END();

  // A few stress tests
  big_test(v1, 1000);
  vect_delete(v1);

  v1 = vect_new();
  big_test(v1, 10000);
  vect_delete(v1);

  v1 = vect_new();
  big_test(v1, 100000);
  vect_delete(v1);

  if (bigger) {
    v1 = vect_new();
    big_test(v1, 1000000);
    vect_delete(v1);

    v1 = vect_new();
    big_test(v1, 10000000);
    vect_delete(v1);

    v1 = vect_new();
    big_test(v1, 100000000);
    vect_delete(v1);
  }

  return 0;
}
