#ifndef STRUCTS_H
#define STRUCTS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define DIR_NAME_LENGTH 48

typedef struct file_entry {
  int mode; // file mode
  char path[100]; // path to file
  char data[100]; // 100 bytes of data per file
} file;

typedef struct dirent {
  char name[DIR_NAME_LENGTH];
  int inum;
  char _reserved[12];
  char used;
} dirent_t;

typedef struct directory {
  int inum; // inode number
  char path[100]; // path to directory
  dirent_t entries[10]; // 10 entries per directory
  int used; // whether or not this directory is used
} direct;

typedef union data {
  file file;
  direct dir;
} data;

typedef struct inode {
    int inum; // inode number
    int refs; // number of references to this inode
    int mode; // file mode
    int size; // file size
    int uid; // user id
    time_t atime; // last access time
    time_t mtime; // last modification time
    time_t ctime; // last status change time
    int direct[12]; // direct data blocks
    int indirect; // indirect data block
} inode_t;


#endif
