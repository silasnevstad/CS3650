#define _DEFAULT_SOURCE
#define _BSD_SOURCE 
#include <malloc.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <debug.h> // definition of debug_printf
#include <sys/mman.h> // mmap, munmap


typedef struct free_block {
  size_t size;
  struct free_block *next;
} free_block;

void *mymalloc(size_t size);
void myfree(void *ptr);
void *mycalloc(size_t nmemb, size_t size);
void coalesce_buckets(int index);
void add_to_buckets(free_block *block);
void *remove_from_buckets(size_t size);

#define THREADS 8 // number of threads

const size_t PAGE_SIZE = 4096; // size of the page as fixed value

__thread free_block *buckets[THREADS] = {NULL}; // array of buckets for each thread (each thread has its own sized bucket) {32, 64, 128, 256, 512, 1024, 2048, 4096}

__thread heap_stats stats; // stats for the heap manager


/*
 * Prints the lengths of the free lists for each bucket.
 */
void print_buckets_length() {
  int list_values[THREADS];

  for (int i = 0; i < THREADS; i++) {
    int count = 0;
    list_values[i] = i == 0 ?  32 : list_values[i-1] * 2;
    
    free_block *curr = buckets[i];

    while (curr != NULL) {
      count++;
      curr = curr->next;
    }

    printf("Free list length for size: %d, %d\n", list_values[i], count);
  }
}

/*
 * Print the stats for the heap manager.
 */
void print_stats() {
    printf("Printing heaper stats:\n");
    printf("Total Mapped: %ld\n", stats.pages_mapped);
    printf("Total Unmapped: %ld\n", stats.pages_unmapped);
    printf("Total Allocs: %ld\n", stats.total_allocs);
    printf("Total Frees: %ld\n", stats.total_frees);
    print_buckets_length();
}

/*
 * Return the number of pages needed to allocate the size.
 */
size_t calculate_pages(size_t size) {
  size_t pages = size / PAGE_SIZE;  
  if (size % PAGE_SIZE != 0) {
      pages++;
  }
  
  return pages;
}

/*
 * Returns the power of two that is greater than or equal to size
 */
static size_t calculate_size(size_t size) {
  size_t power = 32;
  while (power < size) {
    power *= 2;
  }
  return power;
}

/*
 * Coalesce the bucket at the given index.
 *
 * Does this by iterating through the bucket and coalescing the free adjacent blocks.
 * Once the coalescing is done, it adds the coalesced blocks of memory to their appropriate buckets.
 */
void coalesce_buckets(int index) {
  free_block *curr = buckets[index];
  free_block *next = curr->next;

  while (next != NULL) { // while there is a next block check to see if they can be coalesced
    if (curr + curr->size == next) {
      curr->size += next->size;
      curr->next = next->next;
      next = curr->next;
    } else {
      curr = next;
      next = curr->next;
    }
  }

  /* since we just coalesced a bucket we have to add the
   * larger blocks to their larger-sized buckets */ 
  curr = buckets[index];
  free_block *prev = NULL;
  int bucket_size = 32;
  for (int i = 0; i < index; i++) {
    bucket_size *= 2;
  }

  while (curr != NULL) {
    if (curr->size > bucket_size) { // if the block is larger than the bucket size (we have to move it)
      if (prev == NULL) { // if the block is the first block in the bucket
        buckets[index] = curr->next; // we update the bucket to point to the next block
      } else { // else we update the previous block to point to the next block
        prev->next = curr->next;
      }
      free_block *move = curr; // we move the block to the correct bucket
      move->next = NULL;
      add_to_buckets(move);
    } else {
      prev = curr;
    }
    curr = curr->next;
  }
}

/*
 * Adds a given free_block to the buckets, based on its size
 * If the bucket is empty, adds the free_block to the bucket front.
 * If the bucket is not empty, adds the free_block to the back of the bucket.
 */
void add_to_buckets(free_block *block) {
    int s = block->size;

    while (s > 0) {
        int curr_size = calculate_size(s);
        int index = 0;

        while (curr_size > 32) { // calculate the index of the bucket
            curr_size /= 2;
            index++;
        }
        curr_size = calculate_size(s);

        free_block *new_block = (free_block *) block;
        new_block->size = curr_size;
        new_block->next = NULL;

        if (buckets[index] == NULL) { // if the bucket is empty
            buckets[index] = new_block;
        } else { // else add to the back of the bucket
            free_block *curr = buckets[index];
            while (curr->next != NULL) { // find the last block in the bucket
                curr = curr->next;
            }
            curr->next = new_block;
        }

        s -= curr_size; // update the size of the provided block

        if (index != THREADS-1) { // if the block is not the last bucket
            coalesce_buckets(index); // coalesce the buckets
        }
    }
}

/*
 * Given a requested size, remove and return the first free_block from the bucket that satisfies the request
 */
void *remove_from_buckets(size_t size) {
    int index = 0;
    while (size > 32) { // calculate the index of the bucket
        size /= 2;
        index++;
    }

    free_block *curr = buckets[index];

    if (curr == NULL) { // if the bucket is empty
        for (int i = index+1; i < THREADS-1; i++) { // find the next non-empty bucket
            if (buckets[i] != NULL) {
                free_block *new_block = buckets[i];
                buckets[i] = buckets[i]->next;
                new_block->next = NULL;

                size_t leftover_size = new_block->size - size;
                new_block->size = size;

                // add the leftover back to the buckets
                free_block *leftover = (free_block *) ((char *) new_block + size);
                leftover->size = leftover_size;
                leftover->next = NULL;
                add_to_buckets(leftover);

                return (void*)new_block;
            }
        }
    } else { // else remove the first block from the bucket
        buckets[index] = curr->next;
        return (void*)curr;
    }
}

/*
 * Calculates the number of pages and required size for the given size.
 */
void get_size_and_pages(size_t size, size_t *size_to_allocate, size_t *pages) {
  if (size <= 32) {
    *size_to_allocate = 32;
    *pages = 1;
  } else {
    *size_to_allocate = calculate_size(size);
    *pages = calculate_pages(size);
  }
}

/*
 * Allocates a block of memory of the given size by using free blocks from the buckets or by mapping new pages.
 */
void *mymalloc(size_t size) {
    assert(size > 0);
    stats.total_allocs++;

    size_t size_to_allocate;
    size_t pages;
    get_size_and_pages(size, &size_to_allocate, &pages);


    if (size_to_allocate < PAGE_SIZE && size_to_allocate <= 2048) { // if the size is less than 2048
        void *block = remove_from_buckets(size_to_allocate); // remove the block from the buckets
        if (block != NULL) { // we found a block in the buckets
            return (void*)((char*)block + sizeof(size_t));
        } else { // we did not find a block in the buckets so we have to allocate a new page
            void *page = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
            assert(page != MAP_FAILED);
            stats.pages_mapped++;
            stats.total_mapped += PAGE_SIZE;

            free_block *new_block = (free_block*)page;
            new_block->size = size_to_allocate;
            new_block->next = NULL;

            // add the leftover to the buckets
            free_block *leftover = (free_block*) ((char*) page + size_to_allocate);
            leftover->size = PAGE_SIZE - size_to_allocate;
            leftover->next = NULL;
            add_to_buckets(leftover);

            return (void*)((char*)new_block + sizeof(size_t));
        }
    } else { // if the size is greater than 2048
        if (size_to_allocate > 2048 && size_to_allocate < PAGE_SIZE) { // if the size is greater than 2048 but less than a page
            size_to_allocate = PAGE_SIZE;
            pages = 1;
        }
        void *page = mmap(NULL, pages * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        assert(page != MAP_FAILED);

	      size_t *sizePtr = page;
        *sizePtr = pages * PAGE_SIZE;
        sizePtr += 1;
        void* ret = sizePtr;
        return ret;
    }
}

/*
 * Frees the memory pointed to by ptr by adding it to the appropriate bucket
 */
void myfree(void *ptr) {
    assert(ptr != NULL);
    stats.total_frees++;

    free_block *block = (free_block*)((char*)ptr - sizeof(size_t));

    if (block->size < PAGE_SIZE) { // if the block is less than a page
        block->next = NULL;
        add_to_buckets(block);
    } else { // if the block is greater than a page
        munmap(block, block->size);
    }
}

/*
 * Allocates a block of memory of the given size and initializes it to 0.
 */
void *mycalloc(size_t nmemb, size_t size) {
    assert(nmemb > 0 && size > 0);
    stats.total_callocs++;

    long n = nmemb;
    long si = size;
    long s = n * si;

    void *ptr = mymalloc(s);

    size_t size_to_allocate;
    size_t pages;
    get_size_and_pages(s, &size_to_allocate, &pages);

    if (size_to_allocate < PAGE_SIZE && size_to_allocate <= 2048) { // if the size is less than a page and less than 2048
        memset(ptr, 0, s);
    } else { // if the size is greater than a page or greater than 2048
        if (size_to_allocate > 2048 && size_to_allocate < PAGE_SIZE) { // if the size is greater than 2048 but less than a page
            size_to_allocate = PAGE_SIZE;
            pages = 1;
        }
        for (int i = 0; i < pages; i++) {
	          if (i == pages - 1) {
		            for (int j = 0; j < s - (pages - 1) * PAGE_SIZE; j++) {
		                *((char*)ptr + (pages - 1) * PAGE_SIZE + j) = 0;
                }
            } else {
                memset((char*)ptr + i * PAGE_SIZE, 0, PAGE_SIZE);
            }
        }
    }
    return ptr;
}
