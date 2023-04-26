/**
 * Vector implementation.
 *
 * - Implement each of the functions to create a working growable array (vector).
 * - Do not change any of the structs
 * - When submitting, You should not have any 'printf' statements in your vector 
 *   functions.
 *
 * IMPORTANT: The initial capacity and the vector's growth factor should be 
 * expressed in terms of the configuration constants in vect.h
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "vect.h"

/** Main data structure for the vector. */
struct vect {
  char **data;             /* Array containing the actual data. */
  unsigned int size;       /* Number of items currently in the vector. */
  unsigned int capacity;   /* Maximum number of items the vector can hold before growing. */
};

/** Construct a new empty vector. */
vect_t *vect_new() {

  vect_t *v = (struct vect *) malloc(sizeof(struct vect));
  v->data = malloc(2 * sizeof(char));
  v->size = 0;
  v->capacity = 2;
  return v;
}

/** Delete the vector, freeing all memory it occupies. */
void vect_delete(vect_t *v) {
  free(v);
}

/** Get the element at the given index. */
const char *vect_get(vect_t *v, unsigned int idx) {
  assert(v != NULL);
  assert(idx < v->size);

  const char *item = v->data[idx];
  return item;
}

/** Get a copy of the element at the given index. The caller is responsible
 *  for freeing the memory occupied by the copy. */
char *vect_get_copy(vect_t *v, unsigned int idx) {
  assert(v != NULL);
  assert(idx < v->size);
  
  char *item = v->data[idx];
  char *copy = &item;
  return copy;
}

/** Set the element at the given index. */
void vect_set(vect_t *v, unsigned int idx, const char *elt) {
  assert(v != NULL);
  assert(idx < v->size);

  const char *item = elt;
  v->data[idx] = item;
}

/** Add an element to the back of the vector. */
void vect_add(vect_t *v, const char *elt) {
  assert(v != NULL);

  if (v->size == v->capacity) {
    char **tmp = realloc(v->data, 2 * v->size);
    if (tmp) {
      v->data = tmp;
      int newCap = v->size * 2;
      v->capacity = newCap;
    }
  }

  const char *item = elt;
  v->data[v->size] = item;
  v->size = v->size + 1;
}

/** Remove the last element from the vector. */
void vect_remove_last(vect_t *v) {
  assert(v != NULL);
 
  v->size = v->size - 1;
  char **tmp = malloc(sizeof(v->size));

  for (int i = 0; i <= v->size; i++) {
    tmp[i] = v->data[i];
  }
   
  v->data = tmp;
}

/** The number of items currently in the vector. */
unsigned int vect_size(vect_t *v) {
  assert(v != NULL);

  int currSize = 0;
  for (int i = 0; i < v->size; i++) {
    if (v->data[i] != NULL) currSize++; 
  } 

  return currSize;
}

/** The maximum number of items the vector can hold before it has to grow. */
unsigned int vect_current_capacity(vect_t *v) {
  assert(v != NULL);

  /* [TODO] Complete the function */

  return v->capacity;
}
