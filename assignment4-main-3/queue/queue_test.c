/**
 * Unit tests for your queue.
 *
 * This file has a simple approach to testing our queue implementation. 
 * Feel free to add your own unit tests, following the examples below.
 *
 * Our autograder will use a superset of the tests below. If your implementation
 * passes these, you have a good indication that the code works and will make
 * the autograder happy.
 */
#include <stdio.h>
#include <string.h>

#ifndef _TESTINFRA_H
#include <assert.h>
#define DO_TEST(x) (assert(x))
#define TEST_BEGIN(description, ...) \
  fprintf(stderr, "TEST: " description " ... ", ##__VA_ARGS__)
#define TEST_END() fputs("OK\n", stderr)
#endif

#include "queue.h"

void test0() {
  TEST_BEGIN("Create and destroy a queue");
  queue_t *test0 = queue_new(30);
  DO_TEST(queue_size(test0) == 0);
  queue_delete(test0);
  TEST_END();
}

void test1() {
  TEST_BEGIN("Enqueue and dequeue a single item, empty, full");

  queue_t* test1 = queue_new(1);
  DO_TEST(queue_empty(test1));
  DO_TEST(!queue_full(test1));

  queue_enqueue(test1, 42);
  DO_TEST(!queue_empty(test1));
  DO_TEST(queue_full(test1));

  DO_TEST(queue_dequeue(test1) == 42);
  DO_TEST(queue_empty(test1));
  DO_TEST(!queue_full(test1));

  queue_delete(test1);

  TEST_END();
}

void test2() {
  TEST_BEGIN("Enqueue and dequeue a single item, testing the size");
  queue_t* test2 = queue_new(1);

  DO_TEST(queue_size(test2) == 0);

  queue_enqueue(test2, 42);
  DO_TEST(queue_size(test2) == 1);

  DO_TEST(queue_dequeue(test2) == 42);
  DO_TEST(queue_size(test2) == 0);

  queue_delete(test2);
  TEST_END();
}

void test3() {
  TEST_BEGIN("10-element queue");
  queue_t *test3 = queue_new(10);

  DO_TEST(queue_empty(test3));

  queue_enqueue(test3, 1);
  queue_enqueue(test3, 2);
  queue_enqueue(test3, 3);
  queue_enqueue(test3, 4);
  queue_enqueue(test3, 5);

  DO_TEST(queue_size(test3) == 5);
  DO_TEST(!queue_empty(test3));
  DO_TEST(!queue_full(test3));

  queue_enqueue(test3, 6);
  queue_enqueue(test3, 7);
  queue_enqueue(test3, 8);
  queue_enqueue(test3, 9);
  queue_enqueue(test3, 10);

  DO_TEST(queue_size(test3) == 10);
  DO_TEST(!queue_empty(test3));
  DO_TEST(queue_full(test3));

  DO_TEST(queue_dequeue(test3) == 1);
  DO_TEST(queue_dequeue(test3) == 2);
  DO_TEST(queue_dequeue(test3) == 3);
  DO_TEST(queue_dequeue(test3) == 4);
  DO_TEST(queue_dequeue(test3) == 5);

  DO_TEST(queue_size(test3) == 5);
  DO_TEST(!queue_empty(test3));
  DO_TEST(!queue_full(test3));

  DO_TEST(queue_dequeue(test3) == 6);
  DO_TEST(queue_dequeue(test3) == 7);
  DO_TEST(queue_dequeue(test3) == 8);
  DO_TEST(queue_dequeue(test3) == 9);
  DO_TEST(queue_dequeue(test3) == 10);

  DO_TEST(queue_empty(test3));
  DO_TEST(!queue_full(test3));

  queue_delete(test3);

  TEST_END();
}

void test4() {
  TEST_BEGIN("32-element queue");

  queue_t *test4 = queue_new(32);

  for (int i = 0; i < 32; i++) {
    queue_enqueue(test4, 1);
    DO_TEST(queue_size(test4) == i + 1);
  }

  for (int i = 0; i < 32; i++) {
    DO_TEST(queue_dequeue(test4) == 1);
    DO_TEST(queue_size(test4) == 31 - i);
  }

  queue_delete(test4);

  TEST_END();
}

void test5() {
  TEST_BEGIN("Multiple enqueues and dequeues, testing wraparound");

  queue_t *test5 = queue_new(12);

  for (int i = 0; i < 8; i++) {
    queue_enqueue(test5, 1);
  }

  DO_TEST(queue_size(test5) == 8);

  for (int i = 0; i < 4; i++) {
    DO_TEST(queue_dequeue(test5) == 1);
  }

  for (int i = 0; i < 8; i++) {
    queue_enqueue(test5, 2);
  }

  DO_TEST(queue_size(test5) == 12);

  for (int i = 0; i < 4; i++) {
    DO_TEST(queue_dequeue(test5) == 1);
  }

  DO_TEST(queue_size(test5) == 8);

  for (int i = 0; i < 4; i++) {
    DO_TEST(queue_dequeue(test5) == 2);
  }

  DO_TEST(queue_size(test5) == 4);

  for (int i = 0; i < 4; i++) {
    queue_enqueue(test5, 3);
  }

  DO_TEST(queue_size(test5) == 8);

  for (int i = 0; i < 4; i++) {
    DO_TEST(queue_dequeue(test5) == 2);
  }

  DO_TEST(queue_size(test5) == 4);

  for (int i = 0; i < 4; i++) {
    DO_TEST(queue_dequeue(test5) == 3);
  }

  DO_TEST(queue_size(test5) == 0);

  queue_delete(test5);

  TEST_END();
}


int main(int argc, char **argv) {
  test0();
  test1();
  test2();
  test3();
  test4();
  test5();

  return 0;
}
