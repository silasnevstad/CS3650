#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hints/inode.h"
#include "helpers/blocks.h"
#include "helpers/bitmap.h"
#include "structs.h"

void print_inode(inode_t* node) {
    printf("inode located at %p\n", node);
    printf("refs: %d\n", node->refs);
    printf("mode: %d\n", node->mode);
    printf("size: %d\n", node->size);
    printf("block: %d\n", node->block);
    printf("atime: %d\n", node->atime);
    printf("mtime: %d\n", node->mtime);
    printf("ctime: %d\n", node->ctime);
    for (int i = 0; i < 12; i++) {
        printf("direct[%d]: %d\n", i, node->direct[i]);
    }
    printf("indirect: %d\n", node->indirect);
}

inode_t *get_inode(int inum) {
    inode_t* inode = get_inode_bitmap() + 32;
    return &inode[inum];
}

int alloc_inode() {
    int inum = 0;

    for (int i = 0; i < 256; i++) {
        if (!bitmap_get(get_inode_bitmap(), i)) { // if inode is not allocated
            bitmap_put(get_inode_bitmap(), i, 1); // allocate inode
            inum = i;
            break; // no longer need to loop because we found an unallocated inode
        }
    }

    inode_t* new_inode = get_inode(inum);
    new_inode->refs = 1;
    new_inode->mode = 0;
    new_inode->size = 0;

    time_t curr_time = time(NULL);

    new_inode->atime = curr_time;
    new_inode->mtime = curr_time;
    new_inode->ctime = curr_time;
    new_inode->direct[0] = alloc_block();

    return inum;
}

// frees the inode and all blocks associated with it
void free_inode(int inum) {
  inode_t* node = get_inode(inum);
  void* bmp = get_inode_bitmap();

  shrink_inode(node, 0);

  free_block(node->direct[0]);

  bitmap_put(bmp, inum, 0);
}

int grow_inode(inode_t* node, int size) {
    for (int i = (node->size / 4096) + 1; i <= size / 4096; i++) { // loop through all blocks that need to be allocated
        if (i < 12) { // if we are in the direct block range
            node->direct[i] = alloc_block();
        } else { // else we are in the indirect block range
            if (node->indirect == 0) { // 
                node->indirect = alloc_block();
            }
            int* indirect = blocks_get_block(node->indirect);
            indirect[i - 12] = alloc_block();
        }
    }
    node->size = size;
    return 0;
}

int shrink_inode(inode_t* node, int size) {
    for (int i = (node->size / 4096); i > size / 4096; i--) {  // loop through all blocks that need to be freed
        if (i < 12) {
            free_block(node->direct[i]);
            node->direct[i] = 0;
        } else {
            int* indirect = blocks_get_block(node->indirect);
            free_block(indirect[i - 12]);
            indirect[i - 12] = 0;

            if (i == 12) { // if we just freed the last indirect block
                free_block(node->indirect); // free the indirect block because it is no longer needed
                node->indirect = 0;
            }
        }
    }
    node->size = size;
    return 0;
}

int inode_get_bnum(inode_t* node, int fbnum) {
    int bnum = fbnum / 4096;
    if (bnum < 12) { // if we are in the direct block range
        return node->direct[bnum];
    } else {
        int* indirect = blocks_get_block(node->indirect);
        return indirect[bnum - 12];
    }
}

// decreases the reference count of the inode, and frees it if there are no more references
void decrease_refs(int inum) {
    inode_t* node = get_inode(inum);
    node->refs--;
    if (node->refs == 0) { // if there are no more references to the inode
        free_inode(inum);
    }
}
