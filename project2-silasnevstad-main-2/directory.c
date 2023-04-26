#include "helpers/slist.h"
#include "helpers/blocks.h"
#include "hints/inode.h"
#include "hints/directory.h"
#include "helpers/bitmap.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "structs.h"


void directory_init() {
  inode_t* root = get_inode(alloc_inode());
  root->mode = 040755; // directory mode (rwxr-xr-x)
}

int directory_lookup(inode_t *dd, const char *name) {
  if (!strcmp(name, "")) {
    printf("this is the root directory\n");
    return 0;
  }

  dirent_t* subdirs = blocks_get_block(dd->direct[0]);
  for (int i = 0; i < 64; i++) {
    dirent_t curr_dir = subdirs[i];
    if (curr_dir.used && strcmp(curr_dir.name, name) == 0) {
      //printf("found directory %s at index %d\n", name, i);
      return curr_dir.inum;
    }
  }
  
  return -ENOENT; // directory not found
}


int tree_lookup(const char* path) {
  int inum = 0;

  slist_t* path_list = s_explode(path, '/');
  slist_t* curr_dir = path_list;

  while (curr_dir != NULL) {
    inum = directory_lookup(get_inode(inum), curr_dir->data);
    if (inum < 0) { // directory not found
        s_free(path_list);
        return -1;
    }
    curr_dir = curr_dir->next;
  }
  s_free(path_list);
    
  return inum;
}

int directory_put(inode_t* dd, const char* name, int inum) {
  dirent_t *subdirs = blocks_get_block(dd->direct[0]); // get the first block of the directory
  int num_blocks = dd->size / sizeof(dirent_t); // number of blocks in the directory

  dirent_t new;
  new.inum = inum;
  new.used = 1;
  strncpy(new.name, name, DIR_NAME_LENGTH);

  for (int i = 1; i < num_blocks; i++) { // loops through all the blocks in the directory
    if (!subdirs[i].used) { // if the block is not used, we can use it
      subdirs[i] = new;
      return 0;
    }
  }

  // if we get here, we need to allocate a new block
  subdirs[num_blocks] = new;
  dd->size += sizeof(dirent_t);

  return 0;
}

int directory_delete(inode_t* dd, const char* name) {
  dirent_t *subdirs = blocks_get_block(dd->direct[0]);
  int num_blocks = dd->size / sizeof(dirent_t); // number of blocks in the directory

  for (int i = 0; i < num_blocks; i++) { 
    if (subdirs[i].used && strcmp(subdirs[i].name, name) == 0) { // if the block is used and the name matches
      subdirs[i].used = 0; // mark the block as unused
      decrease_refs(subdirs[i].inum); // decrease the number of references to the inode
      return 0;
    }
  }

  printf("could not find directory %s to remove\n", name);
  return -ENOENT;
}

slist_t* directory_list(const char* path) {
  int working_dir = tree_lookup(path); // get the inode number of the directory
  inode_t* node = get_inode(working_dir); // get the inode of the directory
  int num_blocks = node->size / sizeof(dirent_t); // number of blocks in the directory
  dirent_t *subdirs = blocks_get_block(node->direct[0]); // get the first block of the directory
  slist_t* list = NULL;

  for (int i = 0; i < num_blocks; i++) {
    if (subdirs[i].used) { // if the block is used, add it to the list
      list = s_cons(subdirs[i].name, list);
    }
  }

  return list;
}

void print_directory(inode_t* dd) {
    int num_blocks = dd->size / sizeof(dirent_t);
    dirent_t *subdirs = blocks_get_block(dd->direct[0]);

    for (int i = 0; i < num_blocks; i++) {
        if (subdirs[i].used) { // if the block is used, print it
            printf("directory %s at index %d\n", subdirs[i].name, i);
        }
    }
}
