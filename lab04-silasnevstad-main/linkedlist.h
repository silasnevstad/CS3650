#ifndef _LINKEDLIST_H
#define _LINKEDLIST_H

// TODO: Insert your node type definition here
typedef struct node_t {
  int data;
  struct node_t* next;
} node_t;


node_t *cons(int data, node_t *list);
int first(node_t *list);
node_t *rest(node_t *list);
int is_empty(node_t *list);
void print_list(node_t *list);
void free_list(node_t *list);


#endif /* _LINKEDLIST_H */
