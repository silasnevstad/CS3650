# Install and Explore FUSE: Filesystem in Userspace

In this lab you will install [FUSE](https://en.wikipedia.org/wiki/Filesystem_in_Userspace) - a filesystem driver framework that you will use for Project 2. We will also explore the starter code for the project. Do this on your XOA VM.

## Step 1: Install FUSE

For this assignment you will need to use your local Ubuntu VM (or another local
modern Linux). You'll need to install the following packages:
- `libfuse-dev`
- `libbsd-dev`
- `pkg-config`

Running
```
$ sudo apt-get install libfuse-dev libbsd-dev pkg-config
```
should do the trick.



### Provided Makefile and Tests

The provided [Makefile](Makefile)  supplies the following targets:

- `make nufs` - compile the `nufs` binary. This binary can be run manually as follows:
  
  ```
  $ ./nufs [FUSE_OPTIONS] mount_point disk_image
  ```
- `make mount` - mount a filesystem (using `data.nufs` as the image) under `mnt/` in the current directory
- `make unmount` - unmount the filesystem
- `make gdb` - same as `make mount`, but run the filesystem in GDB for debugging
- `make clean` - remove executables and object files, as well as test logs and the `data.nufs`.

## Step 2:  Exploring FUSE with nufs

### Setting up a run/test environment

To run `nufs`, you will need **two** terminal windows.  You'll run the filesystem in one window (`make mount`) and send filesystem commands to it from another one, e.g., `ls mnt/`.  It's best if both windows are visible at all times. 

If you are using Vagrant, open onother terminal window then use the following command:
```shell
$ vagrant ssh
```
For those using QEMU, you can connect another terminal window to your VM with this command:
```shell
$ ssh -p 2200 vagrant@localhost
```

### Exploring 
Let's explore the functionality in [`nufs.c`](nufs.c), which only provides a simulated filesystem. 

Change to the diretory with the lab sources in both windows.
In one window, which we will call the nufs window, type `make nufs`.  
When it completes type `make mount`.

In the other window, the command window, type `ls`.  Note that a new directory, `mmt` was created by `nufs`.  
In the nufs window, you see something like this:
```
$ make mount
mkdir -p mnt || true
./nufs -s -f mnt data.nufs
TODO: mount data.nufs as data file
getattr(/) -> (0) {mode: 40755, size: 0}
```

In the command window, type '`ls -ld mnt`.  You should see the following:
```
$ ls -ld mnt
drwxr-xr-x 0 vagrant root 0 Jan  1  1970 mnt
```
In the nufs window you should see the following new line:
```
getattr(/) -> (0) {mode: 40755, size: 0}
```
BTW, 'mnt' is an alias for '/'.  We see 'mnt'.  FUSE sees '/'

Are there any simialarities?  
 - The size  reported by both to be 0
 - `mode: 40755` :== `drwxr-xr-x`

Looking deeper, type `stat mnt` in the command window:
```
$ stat mnt
  File: mnt
  Size: 0         	Blocks: 0          IO Block: 4096   directory
Device: 34h/52d	Inode: 1           Links: 0
Access: (0755/drwxr-xr-x)  Uid: ( 1000/ vagrant)   Gid: (    0/    root)
Access: 1970-01-01 00:00:00.000000000 +0000
Modify: 1970-01-01 00:00:00.000000000 +0000
Change: 1970-01-01 00:00:00.000000000 +0000
 Birth: -
```

In the nufs window, that following line was added
```
getattr(/) -> (0) {mode: 40755, size: 0}
```

It looks like all the dates for `mnt` are set to the the same date, 1970-01-01 00:00:00.000000000 +0000, the beginning of Unix!  Has `mnt` been around that long?

Type `ls -l mnt` in the command window.  You should see the following:
```
$ ls -l mnt
total 0
-rw-r--r-- 0 vagrant root 6 Jan  1  1970 hello.txt
```

In the nufs window, you shoud see the following lines added:
```
getattr(/) -> (0) {mode: 40755, size: 0}
getattr(/) -> (0) {mode: 40755, size: 0}
getattr(/hello.txt) -> (0) {mode: 100644, size: 6}
readdir(/) -> 0
getattr(/hello.txt) -> (0) {mode: 100644, size: 6}
```

Once more, we see parallel similarities between what `ls -l mnt` returns and what is displayed in the nufs window. 

What's going on here?  

- getattr() is called a lot!
- getattr() seems to return metadata about files, `/` and `/hello.txt`  
- readdir() appears to return the contents of the `/`

Let's look in [`nufs.c`](nufs.c). `main` looks innocouous. 
``` c
   199	int main(int argc, char *argv[]) {
   200	  assert(argc > 2 && argc < 6);
   201	  printf("TODO: mount %s as data file\n", argv[--argc]);
   202	  // storage_init(argv[--argc]);
   203	  nufs_init_ops(&nufs_ops);
   204	  return fuse_main(argc, argv, &nufs_ops, NULL);
   205	}
```
Since getattr() is called frequently, let's look at that.  
``` c

    32	// Gets an object's attributes (type, permissions, size, etc).
    33	// Implementation for: man 2 stat
    34	// This is a crucial function.
    35	int nufs_getattr(const char *path, struct stat *st) {
    36	  int rv = 0;
    37
    38	  // Return some metadata for the root directory...
    39	  if (strcmp(path, "/") == 0) {
    40	    st->st_mode = 040755; // directory
    41	    st->st_size = 0;
    42	    st->st_uid = getuid();
    43	  }
    44	  // ...and the simulated file...
    45	  else if (strcmp(path, "/hello.txt") == 0) {
    46	    st->st_mode = 0100644; // regular file
    47	    st->st_size = 6;
    48	    st->st_uid = getuid();
    49	  } else { // ...other files do not exist on this filesystem
    50	    rv = -ENOENT;
    51	  }
    52	  printf("getattr(%s) -> (%d) {mode: %04o, size: %ld}\n", path, rv, st->st_mode,
    53	         st->st_size);
    54	  return rv;
    55	}
```

We see that the two files `/` and `/hello.txt` are simulated and `getattr()` returns information in a `struct stat` (`man fstat`).  So if they are simulated in `getattr()`, other functions need to simulate them too.  Let's try a few more things. 

In the command window, type `$ cat mnt/hello.txt`
```
$ cat mnt/hello.txt
hello
```

In the nufs window, you should see the following lines added:
```shell
getattr(/hello.txt) -> (0) {mode: 100644, size: 6}
open(/hello.txt) -> 0
read(/hello.txt, 4096 bytes, @+0) -> 6
```

This output seems to indicate that `/hello.txt` is a regular file that contains 6 bytes `hello\n`, has the mode 100644, and all the calls returned successfully.

Can we create a file in `mnt`?  Type `touch mnt/test.txt`:
```shell
$ touch mnt/test.txt
touch: setting times of 'mnt/test.txt': No such file or directory
```

I guess not.  The following lines are added to the nufs window:
``` shell
getattr(/test.txt) -> (-2) {mode: 0000, size: 0}
mknod(/test.txt, 100664) -> -1
getattr(/test.txt) -> (-2) {mode: 0000, size: 0}
```

The lines show that `getattr` identfied that `test.txt` didn't exist, but `mknod` was unable to create the file.

Can we modify a file in 'mnt`?  Type `echo world >> mnt/hello.txt`:
``` shell
$ echo world >> mnt/hello.txt
-bash: echo: write error: Operation not permitted
```

Looks like modifying/writing files is not permitted either.  The following lines are added to the nufs window:
``` shell
getattr(/) -> (0) {mode: 40755, size: 0}
getattr(/hello.txt) -> (0) {mode: 100644, size: 6}
open(/hello.txt) -> 0
write(/hello.txt, 6 bytes, @+6) -> -1
```

Here we failed at writing the modification to `hello.txt`.  The negative numbers  returned from the functions indicate errors. 

If we look again at   [`nufs.c`](nufs.c), we notice that most of the functions are only stubbed out and that they return a negative number to indicate the desired functionality is not present.  The functions that have some functionality have special cases for '/' and '/hello.txt'.  Of the 16 API calls in [`nufs.c`](nufs.c), only `nufs_access`, `nufs_getattr`, `nufs_readdir` have hardcoded information about  '/' and '/hello.txt.

``` c
    16	// implementation for: man 2 access
    17	// Checks if a file exists.
    18	int nufs_access(const char *path, int mask) {
    19	  int rv = 0;
    20
    21	  // Only the root directory and our simulated file are accessible for now...
    22	  if (strcmp(path, "/") == 0 || strcmp(path, "/hello.txt") == 0) {
    23	    rv = 0;
    24	  } else { // ...others do not exist
    25	    rv = -ENOENT;
    26	  }
    27
    28	  printf("access(%s, %04o) -> %d\n", path, mask, rv);
    29	  return rv;
    30	}    
```

``` c
    57	// implementation for: man 2 readdir
    58	// lists the contents of a directory
    59	int nufs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
    60	                 off_t offset, struct fuse_file_info *fi) {
    61	  struct stat st;
    62	  int rv;
    63
    64	  rv = nufs_getattr("/", &st);
    65	  assert(rv == 0);
    66
    67	  filler(buf, ".", &st, 0);
    68
    69	  rv = nufs_getattr("/hello.txt", &st);
    70	  assert(rv == 0);
    71	  filler(buf, "hello.txt", &st, 0);
    72
    73	  printf("readdir(%s) -> %d\n", path, rv);
    74	  return 0;
    75	}
```
### Compare

Compare the output from `strace nufs.c` with `strace mnt/hello.c`.  Are there any significant differnences? 

### Add a simulated file

Modify `nufs_access`, `nufs_getattr`, `nufs_readdir` to add the simulated file `/test.txt` with the contents `0123456789`.

### Change a name

Change the name of `hello.txt` to `myworld.txt` and the contents to `myworld`.  

### Adding a simulated directory

Examine 'nufs.c'.  What would it take to add an empty subdirectory `/subdir` to `/`?  Try to implement your idea?  Does it work?  If not, what do you think was missing from your initial design?

What would it take to add a file `/subdir/test2.txt`? 

## Summary

We've only been simulating file in nufs.c.  There is no real persistence, because everything is hardcoded.  In Assignment 12, you will create a dynamic and persistent memory-mapped file system.  


    




## Deliverable

1. Submit your Github repo to Gradescope under Lab 12.
2. DO NOT submit any binary files (no executables, no .o files, no disk image files).






## Hints & Tips

 - There aren't man pages for FUSE. Instead, the documentation is in the header
   file: `/usr/include/fuse/fuse.h`
 - The sources for [libfuse](https://github.com/libfuse/libfuse) contains a few further [examples](https://github.com/libfuse/libfuse/tree/master/example). Start with [`hello.c`](https://github.com/libfuse/libfuse/blob/master/example/hello.c).
 - By default, Fuse is multithreaded. That's handy for production filesystems, because it lets client (or file access) A proceed even if client B is hung up. But multithreading introduces the possibility of race conditions, and makes debugging harder. **Always run with the -s switch to avoid this problem.**
 - Your `printf/fprintf` debugging code will only work if you run with the -f switch. Otherwise, Fuse disconnects `stdout` and `stderr`.
 - Remember, the basic development / testing strategy for this assignment is to run your
   program (e.g., using `make mount`) in one terminal window and try file system operations on the mounted filesystem in **another separate terminal window**.
 - Read the manual pages for the system calls you're implementing.
 - To return an error from a FUSE callback, you return it as a negative number
   (e.g. return -ENOENT). Some things don't work if you don't return the right
   error codes.
 - Another resource: <https://www.cs.hmc.edu/~geoff/classes/hmc.cs135.201109/homework/fuse/fuse_doc.html>

