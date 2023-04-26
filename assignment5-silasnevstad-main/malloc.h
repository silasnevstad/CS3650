#ifndef _MALLOC_H
#define _MALLOC_H

/* This header is redefining every call to 'malloc' to our implementation
 * 'mymalloc'
 * Again, this is a simple textual replacement of the code by the preprocessor
 */

#include <stddef.h>

#define malloc(size) mymalloc(size)
#define calloc(nmemb, size) mycalloc(nmemb, size)
#define free(ptr) myfree(ptr)

void *mymalloc(size_t size);
void *mycalloc(size_t nmemb, size_t size);
void myfree(void *ptr);

#endif /* ifndef _MALLOC_H */
