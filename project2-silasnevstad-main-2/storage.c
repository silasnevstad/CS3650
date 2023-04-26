#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "helpers/slist.h"
#include "helpers/blocks.h"
#include "hints/directory.h"
#include "hints/inode.h"
#include "helpers/bitmap.h"
#include "structs.h"

static void get_parent_child(const char* path, char* parent, char* child);

void storage_init(const char* path) {
    blocks_init(path); // initialize blocks

    if (!bitmap_get(get_blocks_bitmap(), 1)) { // if inode bitmap is not initialized
        for (int i = 0; i < 12; i++) {
            int new_block = alloc_block();
            printf("inode block allocated at block %d", new_block);
        }
    }

    if (!bitmap_get(get_blocks_bitmap(), 4)) { // if root directory is not initialized
        printf("initializing root directory");
        directory_init(); // initialize root directory
    }
}

int storage_access(const char* path) {
    int rv = tree_lookup(path);
    if (rv >= 0) { // if path exists
        inode_t* node = get_inode(rv); // get inode
        node->atime = time(NULL); // update access time
        return 0;
    } else { // if path does not exist
        return -ENOENT;
    }
}


int storage_stat(const char* path, struct stat* st) {
    int working_inum = tree_lookup(path);

    if (working_inum > 0) { // if path exists
        inode_t* node = get_inode(working_inum);
        st->st_mode = node->mode;
        st->st_size = node->size;
        st->nlink = node->refs;
        st->atime = node->atime;
        st->mtime = node->mtime;
        st->ctime = node->ctime;
        return 0;
    }

    return -ENOENT; // path does not exist
}

int storage_read(const char* path, char* buf, size_t size, off_t offset) {
    inode_t* read_node = get_inode(tree_lookup(path)); // get inode

    int index = 0; // index into buf
    int n = offset; // offset into file
    int rem = size; // remaining bytes to read

    while (rem > 0) { // while there are still bytes to read
        char* src = blocks_get_block(inode_get_pnum(read_node, n));
        src += n % 4096; // get the offset into the block
        int cpyamt = min(rem, 4096 - (n % 4096)); // amount to copy
        memcpy(buf + index, src, cpyamt); // copy the data
        rem -= cpyamt; // decrement remaining bytes
        n += cpyamt; // increment offset
        index += cpyamt; // increment index
    }

    return size;
}

int storage_write(const char* path, const char* buf, size_t size, off_t offset) {
    inode_t* write_node = get_inode(tree_lookup(path));

    if (write_node->size < offset + size) { // if the file needs to be grown
        storage_truncate(path, offset + size);
    }

    int index = 0; // index into buf
    int off = offset; // offset into file
    int rem = size; // remaining bytes to write

    while (rem > 0) {
        char* dest = blocks_get_block(inode_get_pnum(write_node, off)); // get the block
        dest += off % 4096; // get the offset into the block
        int cpyamt = min(rem, 4096 - (n % 4096)); // get the amount to copy
        memcpy(dest, buf + index, cpyamt); // copy the data
        rem -= cpyamt; // decrement remaining bytes
        off += cpyamt; // increment offset
        index += cpyamt; // increment index
    }

    return size; // return the amount of bytes written
}

// used to grow or shrink a file
int storage_truncate(const char* path, off_t size) {
    int inum = tree_lookup(path);
    inode_t* node = get_inode(inum);

    if (node->size < size) { // if the node size is less than the size then grow the node
        grow_inode(node, size); 
    } else { // else shrink the node
        shrink_inode(node, size);
    }

    return 0;
}

// used to create a new file or directory
int storage_mknod(const *char path, int node) {
    if (tree_lookup(path) != -1) { // if the path already exists
        return -EEXIST;
    }

    char item = malloc(50);
    char parent = malloc(strlen(path)); // allocate space for parent and child
    get_parent_child(path, parent, item); // get the parent and child
    int parent_inum = tree_lookup(parent);

    if (parent_inum < 0) { // if the parent does not exist
        free(item);
        free(parent);
        return -ENOENT;
    }

    // here we are allocating a new inode, initializing it and adding it to the parent directory
    int new_inum = alloc_inode(); 
    inode_t* new_node = get_inode(new_inum);
    new_node->mode = node;
    new_node->refs = 1;
    new_node->size = 0;
    inode_t* parent_dir = get_inode(parent_inum);

    directory_put(parent_dir, item, new_inum);
    free(item); // free the allocated space
    free(parent);

    return 0;
}


int storage_unlink(const char* path) {
    char* item = malloc(50);
    char* parent = malloc(strlen(path));
    get_parent_child(path, parent, item);

    inode_t* parent_dir = get_inode(tree_lookup(parent));
    int rv = directory_delete(parent_dir, item); // delete the item from the parent directory

    free(item); // free the allocated space
    free(parent);

    return rv;
}

// used to create a hard link
int storage_link(const char* from, const char* to) {
    int to_inum = tree_lookup(to);
    
    if (to_inum < 0) { // if the to path does not exist
        return -ENOENT;
    }

    char* from_item = malloc(50);
    char* from_parent = malloc(strlen(from));
    get_parent_child(from, from_parent, from_item);

    inode_t* from_dir = get_inode(tree_lookup(from_parent));
    dirctory_put(from_dir, from_item, to_inum);
    get_inode(to_inum)->refs++;

    free(from_item);
    free(from_parent);

    return 0;
}

// used to rename a file or directory
int storage_rename(const char* from, const char* to) {
    storage_link(from, to);
    storage_unlink(from);
    return 0;
}

// used to set the time on a file
int storage_set_time(const char* path, const struct timespec ts[2]) {
    int inum = tree_lookup(path);

    if (inum < 0) { // if the path does not exist
        return -ENOENT;
    }

    inode_t* node = get_inode(inum);
    node->atime = ts[0].tv_sec;
    node->mtime = ts[1].tv_sec;
    return 0;
}

// used to list the contents of a directory
slist* storage_list(const char* path) {
    return directory_list(path); 
}


// used to update the status time
void storage_update_ctime(const char* path) {
    int inum = tree_lookup(path);
    if (inum < 0) { // if the path does not exist
        return -ENOENT;
    }
    get_inode(inum)->ctime = time(NULL);
}
 
// used to change the permissions on a file
int storage_chmod(const char* path, mode_t mode) {
    int inum = tree_lookup(path);
    if (inum < 0) { // if the path does not exist
        return -ENOENT;
    }
    get_inode(inum)->mode = get_inode(inum)->mode & ~07777 & mode;
    return 0;
}

// used to get parent and child from a path
static void get_parent_child(const char* path, char* parent, char* child) {
    slist* path_list = s_explode(path, '/');
    slist* path_dir = path_list;
    parent[0] = 0;

    while (path_dir->next != NULL) { // loop through the path and get the parent and child
        strcat(parent, path_dir->data, DIR_NAME_LENGTH); // get the parent
        strcat(parent, "/", 1); // add the slash
        path_dir = path_dir->next;
    }

    memcpy(child, path_dir->data, strlen(path_dir->data));
    child[strlen(path_dir->data)] = 0; // add the null terminator
    s_free(path_list);
}
