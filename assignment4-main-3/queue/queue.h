/**
 * Queue type and function declarations.
 *
 * This file is not to be changed for submission. You may change it while 
 * experimenting, but we will replace it with its original version when testing 
 * your submission.
 */
#ifndef _QUEUE_H
#define _QUEUE_H

/** Our queue type (fields are hidden in the implementation file). */
typedef struct queue queue_t;

/** 
 * Construct a new empty queue.
 *
 * Returns a pointer to a newly created queue.
 * Return NULL on error
 */
queue_t *queue_new(unsigned int capacity);

/**
 * Check if the given queue is empty
 *
 * Returns a non-0 value if the queue is empty, 0 otherwise.
 */
int queue_empty(queue_t *q);

/**
 * Check if the given queue is full.
 *
 * Returns a non-0 value if the queue is empty, 0 otherwise.
 */
int queue_full(queue_t *q);

/** 
 * Enqueue a new item.
 *
 * Push a new item into our data structure.
 */
void queue_enqueue(queue_t *q, long item);

/**
 * Dequeue an item.
 *
 * Returns the item at the front of the queue and removes an item from the 
 * queue.
 *
 * Note: Removing from an empty queue is an undefined behavior (i.e., it could 
 * crash the program)
 */
long queue_dequeue(queue_t *q);

/** 
 * Queue size.
 *
 * Queries the current size of a queue (valid size must be >= 0).
 */
unsigned int queue_size(queue_t *q);

/** 
 * Delete queue.
 * 
 * Remove the queue and all of its elements from memory.
 *
 * Note: This should be called before the proram terminates.
 */
void queue_delete(queue_t *q);


#endif /* ifndef _QUEUE_H */
