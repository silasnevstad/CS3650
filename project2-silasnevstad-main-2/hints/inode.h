// Inode manipulation routines.
//
// Feel free to use as inspiration.

// based on cs3650 starter code
#include <time.h>

#ifndef INODE_H
#define INODE_H

#include "../structs.h"
#include "../helpers/blocks.h"

void print_inode(inode_t *node);
inode_t *get_inode(int inum);
int alloc_inode();
void free_inode();
int grow_inode(inode_t *node, int size);
int shrink_inode(inode_t *node, int size);
int inode_get_bnum(inode_t *node, int fbnum);

#endif
