#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "linkedlist.h"

node_t *cons(int data, node_t *list) {
  node_t* newNode = malloc(sizeof(list));
  //node_t* newNode;
  newNode->data = data;
  newNode->next = list;
  return newNode;
}

int first(node_t *list) {
  assert(list);
  return list -> data;
}

node_t *rest(node_t *list) {
  assert(list);
  return list -> next;
}

int is_empty(node_t *list) {
  if (list->next == NULL) {
    return 0;
  } else {
    return 1;
  }
}

void print_list(node_t *list) {
  //for (; list; list = list -> next) {
  //  printf("%i\n", list -> data);
  //  if (list -> next == NULL) { printf("empty"); }
  //}
  //printf("done.");
  node_t* iterator;
  iterator = list;
 
  while (iterator != NULL) {
    printf("%d\n", iterator->data);
    iterator = iterator->next;
  }
  
  //while (list->next != NULL) {
  //  printf("%d\n", *list->data);
  //  list = list -> next;
  //}

  //int d = list->data;
  //if (&list->next != NULL) {
  //  printf("%d\n", list->data);
  //  print_list(list = &list -> next);
  //}
}

// TODO: Complete.
void free_list(node_t *list) {
//  if (list) {
//    free_list(list->next);
//    free(list);
//  }

//  node_t* currentNode;
//  while (list != NULL) {
//    currentNode = list;
//    list = list -> next;
//    free(currentNode);
//  }

//  node_t* iterator;
//  iterator = list;
// 
//  while (iterator != NULL) {
//    free(iterator);
//    iterator = iterator->next;
//  }

  if (list) {
    node_t * temp = list;
    free_list(temp->next);
    free(list);
  }
}


