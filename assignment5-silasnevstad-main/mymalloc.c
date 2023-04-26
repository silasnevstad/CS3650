#define _DEFAULT_SOURCE
#define _BSD_SOURCE 
#include <malloc.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <debug.h> // definition of debug_printf

void *mymalloc(size_t size);
void myfree(void *ptr);
void *myrealloc(void *ptr, size_t size);
void *mycalloc(size_t nmemb, size_t size);
void printList();

/* 
  ====================================================================================
  BUGS
  - Prev and next pointers are getting jumbled to random variables
  - FirstBlock prev pointer won't stay NULL
  - Splitting block creates a weird new block and ruins linked list
  - A blocks size will get set to random variables (looks like next or prev pointer)
*/

typedef union block {
  struct {
    size_t size;
    int free;
    union block *next;
    union block *prev;
  } node;
} *Block;

#define	BLOCK_SIZE sizeof(Block); // size of the block as fixed value

static Block firstBlock = NULL; // global variable of the first block in the list
static Block lastBlock = NULL; // global variable of the last (previous) block in the list
static Block currBlock = NULL; // global variable for the current block

// initialize the first block in the linked list with given size and return the block
Block initFirstBlock(size_t size) {
  Block block = sbrk(0); // get the current end of the heap
  int s = size + BLOCK_SIZE; 

  sbrk(s); // increment the heap by the size of the block

  block->node.size = size; // set the size of the block
  block->node.free = 0; // set the block to be in use
  block->node.next = NULL; // set the next block to be NULL
  block->node.prev = NULL; // set the previous block to be NULL

  assert(block->node.free == 0 || block->node.free == 1);

  firstBlock = block; // set the first block to be the block
  lastBlock = block; // set the last block to be the block

  return block;
}

// extend the linked block list with given size
static Block extendList(size_t size) {
  printf("Extending List...\n");

  size_t blockS = BLOCK_SIZE;
  size_t s = size + BLOCK_SIZE;
  if (s < blockS) { // get the requested size (if its smaller than the block size set to block size)
    s = BLOCK_SIZE;
  }

  void *p = sbrk(0); // get the memory from the system
  Block newBlock = p; // allocate memory of given size + block size (min: block size)
  
  assert(sbrk(s) != (void*)-1);

  assert(newBlock != (void*)-1);

  newBlock->node.size = size; // set size of block
  newBlock->node.free = 0; // set block to be used
  newBlock->node.next = NULL; // set next block to be null
  newBlock->node.prev = lastBlock; // set previous block to be last block

  assert(newBlock->node.free == 0 || newBlock->node.free == 1);

  lastBlock->node.next = newBlock; // set last block's next to be new block
  lastBlock = newBlock; // set last block to be new block
  lastBlock->node.next = NULL; // set last block's next to be null
  firstBlock->node.prev = NULL; // set first block's prev to be null

  assert(lastBlock == newBlock);
  assert(newBlock->node.prev == lastBlock->node.prev);
  assert(newBlock->node.next == NULL);
//  assert(newBlock->node.size == size);

  firstBlock->node.prev = NULL;

  return newBlock;
}

// split a block into two blocks, one with given size and one with the rest of the block
static Block splitBlock(Block block, size_t size) {
  printf("Splitting block...\n");
  size_t blockS = BLOCK_SIZE;
  size_t s = size + BLOCK_SIZE;

  if (s < blockS) { // get the requested size (if its smaller than the block size set to block size)
    s = BLOCK_SIZE;
  }

  Block newBlock = (block + 1 + s); // set new block to be the block + the requested size + block size (min: block size)

  newBlock->node.size = block->node.size - s; // set new block size to be the old block size - requested size - block size (min: block size)
  newBlock->node.free = 1; // set new block to be free
  newBlock->node.next = block->node.next; // set new block next to be the old block next
  newBlock->node.prev = block; // set new block prev to be the old block

  block->node.size = size; // set old block size to be the requested size
  block->node.next = newBlock; // set old block next to be the new block

  if (newBlock->node.next != NULL) { // if the new block next is not null
    newBlock->node.next->node.prev = newBlock; // set the new block next's prev to be the new block
  }

  if (lastBlock == block) { // if the last block is the old block
    lastBlock = newBlock; // set the last block to be the new block
  }

  assert(newBlock->node.free == 1);
  assert(block->node.size == size);
  assert(block->node.next == newBlock);

  return newBlock;
}

//  loop through list and coalesce adjacent free blocks
void coalesce() {
  Block p = firstBlock; // set p to be the first block in the list

  while (p != NULL) { // while p is not NULL
    if (p->node.next != NULL) { // if next is not NULL
      if (p->node.next->node.free == 1) { // and there is free space
        p->node.size += p->node.next->node.size + BLOCK_SIZE; // increase the size of this block by the size of the next block
        p->node.next = p->node.next->node.next;

        if (lastBlock == p->node.next) { 
          lastBlock = p; // if this is the last block in the list, then update lastBlock to be the previous block
        }

        if (currBlock == p->node.next) {
          currBlock = p; // if this is the current block, then update currentBlock to be the previous block
        }
      }
    }
    p = p->node.next; // update p to be the next block
  }
}

// find a next free block of memory of given size
void* firstFitBlock(size_t size) {
  printf("Find first-fit...\n");

  //currBlock->node.next
  Block ptr = firstBlock; // get the next block in the linked list (where we left off)

  size_t s = size + BLOCK_SIZE;

  assert(ptr != (void*)-1); // assure that the pointer is not null

  while(ptr != NULL) { // loop through blocks

    /* given a block if free, we must check if the size of the block is eqaul to the requested size 
     or if the size of the block  is greater than the requested size, if its equal we can just return 
     the block, but if its greater we must split the block into two blocks, one with the requested
     size and one with the remaining size */

    if (ptr->node.free == 1 && ptr->node.size == size) { // if block is free and sizes match each other
      ptr->node.free = 0; // set to be used
      ptr->node.size; // set size
      currBlock = ptr; // update currentBlock to be the one we just found

      assert(ptr->node.free == 0 || ptr->node.free == 1);

      return ptr;
    }

    if (ptr->node.free == 1 && ptr->node.size > s) { // if block is free but size is bigger than requested size
      ptr = splitBlock(ptr, size); // split block, leaving the renmaining size as a new block
      ptr->node.free = 0; // set to be used
      ptr->node.size = size; // set size

      assert(ptr->node.free == 0 || ptr->node.free == 1);

      return ptr;
    }

  //  if (ptr->node.next != NULL) { // if the next block is not null
  //    if (ptr == currBlock) { 
  //      ptr = NULL; // only reached if we have looped through the entire list
  //    } else {
  //      ptr = ptr->node.next; // otherwise set to be the next block
  //    }
  //  } else { // if the next block is null (end of list)
  //    if (ptr == firstBlock) {
  //      ptr = NULL; // reached only if first block is the only block in the list
  //    } else { 
  //      ptr = firstBlock; // otherwise set to be first block
  //    }
  //  }
    ptr = ptr->node.next;
  }

  return (void*)(extendList(size) + 1);  // increase list by given size and return the new block
                                         // reached only if we have looped through the entire list and no block was found
}

// prints out the linked list of blocks
void print_list() {
  printf("Printing list...\n");
  Block ptr = firstBlock;
  int i = 0;

  while (ptr != NULL) { // loops through linked list
    printf("Block number %d\n", i);
    printf("Block Address: %p\n", ptr);
    printf("Block size: %d\n", ptr->node.size);
    printf("Block free: %d\n", ptr->node.free);
    printf("Block next: %d\n", ptr->node.next);
    printf("Block prev: %d\n\n", ptr->node.prev);
    ptr = ptr->node.next;
    i++;
  }
}

void *mymalloc(size_t s) {
  debug_printf("malloc %zu bytes\n", s);

//  print_list();

  if (firstBlock == NULL) { // if first block is 0, then we have not initialized the linked list yet

    return (void*)(initFirstBlock(s) + 1); // initialize the linked list and return the first block

  } else { // otherwise, we have already initialized the linked list

    Block ptr = firstFitBlock(s); // find a first fit block of memory of given size
    assert(ptr != (void*)-1); // assure that the pointer is not null

    if (ptr == NULL) { // if ptr is NULL, then we have not found a free block of memory (we must increase list)

      return (extendList(s) + 1); // increase the linked list size by size

    } else { // otherwise, we have found a free block of memory

      currBlock = ptr; // update current block to be the one we just allocated memory to

      return ptr; 

    }
  }
}

void *mycalloc(size_t nmemb, size_t s) {
  debug_printf("calloc %zu bytes\n", s);
  size_t size = nmemb * s; // get the total size of the memory to be allocated

  assert(s == size / nmemb); // assure that the size is not too big

  Block ptr = mymalloc(size); // allocate memory of given size using mymalloc() function

  assert(ptr != (void*)-1); // assure that the pointer is not null (mymalloc failed)

  memset(ptr, 0, size); // set the memory to be 0

  return ptr;
}


void myfree(void *ptr) {
  debug_printf("Freed some memory, %p\n", ptr);

//  print_list();

  Block p = (Block)ptr - 1; // get the block that the pointer is pointing to

  assert(p != (void*)-1); // assure that the pointer is not null
  assert(p != NULL);

  p->node.free = 1;  

  /* now we have to check that there is no adjacent free blocks, or if this is the last 
     block in the list. If there are any adjacent free blocks, then we must merge the two blocks
     and if this is the last block in the list, then we must decrease the list size. Otherwise,
     we can just set the block to be free */

  coalesce(); // coalesce the blocks
  
  if (p == lastBlock) { // if this is the last block in the list
    if (lastBlock == firstBlock) { // and if this is the only block in the list
      firstBlock = NULL;
      currBlock = NULL;
      lastBlock = NULL;
      if (brk((void*)(p)) == -1) { // release memory back to the OS
        perror("Error freein memory");
        exit(1);
      }
    } else if (lastBlock == currBlock) { // and if this is the current block
      currBlock = currBlock->node.prev; // update currentBlock to be the previous block
      lastBlock = lastBlock->node.prev; // update lastBlock to be the previous block
      lastBlock->node.next = NULL;
      if (brk((void*)(p-1)) == -1) { // release memory back to the OS
        perror("Error freein memory");
       	exit(1);
      } 
    } else { // otherwise, just update lastBlock to be the previous block
      lastBlock = lastBlock->node.prev; // update lastBlock to be the previous block
      lastBlock->node.next = NULL; 
      if (brk((void*)(p-1)) == -1) { // release memory back to the OS
        perror("Error freein memory");
       	exit(1);
      } 
    }
  }
//  print_list();
// assert(p->free == 1);  // BUG: this assertion fails for some reason (also doesn't show error message just quits program)
}

// extra implementation of realloc() function
void *myrealloc(void *ptr, size_t s) {
  debug_printf("realloc %zu bytes\n", s);

  Block p = (Block)ptr - 1; // get the block that the pointer is pointing to

  assert(p != (void*)-1); // assure that the pointer is not null
  assert(p != NULL);

  if (p->node.size >= s) { // if the size of the block is greater than or equal to the size we want to reallocate to
    return ptr; // then we can just return the pointer
  } else { // otherwise, we must allocate a new block of memory and copy the data over
    Block newBlock = mymalloc(s); // allocate a new block of memory of size s
    memcpy(newBlock, ptr, p->node.size); // copy the data from the old block to the new block
    myfree(ptr); // free the old block
    return newBlock; // return the new block
  }
}
