/* Perform a few list operations. */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "linkedlist.h"

void generate_list_tests(int n) {
  node_t *list1 = NULL;

  puts("Generating a list...");
  for (int i = n - 1; i >= 0; --i) {
    printf("Consing %d\n", i * i);
    list1 = cons(i * i, list1);
  }

  puts("\nGenerated list:");
  print_list(list1);

  node_t *list2 = NULL;
  node_t *iter = list1;

  while (iter != NULL) {
    list2 = cons(first(iter), list2);
    iter = rest(iter);
  }

  puts("\nReversed list:");
  print_list(list2);

  free_list(list1);
  free_list(list2);

  puts("");
}

int main(int argc, char **argv) {
  if (argc == 2) {
    generate_list_tests(atol(argv[1]));
  }
  
  assert(is_empty(NULL));

  puts("\nlist1");
  node_t *list1 = cons(1, NULL);
  assert(!is_empty(list1));
  assert(is_empty(rest(list1)));
  assert(first(list1) == 1);
  assert(rest(list1) == NULL);
  puts("The list should contain 1");
  print_list(list1);
  free_list(list1);
  
  puts("\nlist2");
  node_t *list2 = cons(13, cons(42, NULL));
  assert(!is_empty(list2));
  assert(!is_empty(rest(list2)));
  assert(is_empty(rest(rest(list2))));
  assert(first(list2) == 13);
  assert(first(rest(list2)) == 42);
  puts("The list should contain 13, 42:");
  print_list(list2);
  free_list(list2);
}
