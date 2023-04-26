# Memory Allocators

In this week's lab, you are going to start getting familiar with some of the tools used to implement a memory allocator. A memory allocator in the C programming language is, more broadly, '*a set of functions that support requests to allocate or deallocate memory*.' 

Remember, part of this class is building a concrete mental model of what is really going on inside of a machine (so that there is "no more magic" when you see a computer running). We have looked at how memory is managed by the operating system and hardware, launching processes (with fork and exec), understanding a process' memory space, and now allocating memory.

**Even though this lab is to be submitted individually, feel free to partner up and discuss the lab content with some of your classmates privately.**

## Logistics

For this lab,  use your XOA VM to complete and test your work. The examples I provide below are designed strictly for 64-bit Linux. There is an associated quiz on Canvas: <https://northeastern.instructure.com/courses/119197/quizzes/451497>.

## Part 1 - Brief Recap on using malloc, calloc, and free

Let's first refresh on how to use our current memory allocator which lives in [stdlib.h](http://www.unix.org/version2/sample/stdlib.h.html). Below is an example of using some of the functions. If you feel confident about your malloc-fu, feel free to skip ahead.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main(int argc, char **argv) {
  /* Dynamically allocate some amount of memory */
  char *message = (char *) malloc(sizeof(char) * 12);

  /* Check if memory allocation failed (i.e. we do not have enough bytes */
  if (NULL == message) {
    printf("Allocation failed\n");
    exit(0);
  }

  /* If it did not fail, then copy a string into our allocated memory.
   * Function signature: char *strcpy(char *dest, const char *src); 
   */
  strcpy(message, "Hello, Bob\n");

  printf("%s", message);

  /* Lets now free our memory */
  free(message);

  /* If we are working with data, it can sometimes be nice to zero everything 
   * out. You'll notice the parameters are a little flipped here. This is asking
   * for '57' blocks of memory that are the sizeof(int). So we are getting 57 
   * sizeof(int) pieces of memory). 
   */
  int *numberData = (int *) calloc(57, sizeof(int));
  int i;

  /* Let's confirm it is all zeroed out memory */
  for (i = 0; i < 57; ++i) {
    assert(numberData[i] == 0);
    printf("%d ", numberData[i]);
  }
  printf("\n");

  /* Lets free our data */
  free(numberData);

  return 0;
}
```

## Part 2 - Background Reading on Memory Allocators


Go through the slides on [Dynamic Allocation](DynamicAllocation.pdf) and ask questions if something isn't clear.

Take a moment to read [Cexcerpt.pdf](Cexcerpt.pdf) (2 pages ~5 minutes reading) to get a basic understanding of how allocators work. 



## Part 3 - Allocator building blocks

A few of the key system calls that we will need to understand are the following. Read the man pages and ask for clarification if you are struggling to understand some part of them.


* Type in `man 2 sbrk` in the terminal. Read the description of what sbrk does .

```
BRK(2)                                                 Linux Programmer's Manual

NAME
       brk, sbrk - change data segment size

```

* Type in `man 2 mmap` in the terminal next.

```
MMAP(2)                                                Linux Programmer's Manual

NAME
       mmap, munmap - map or unmap files or devices into memory
```

* Type in `man 2 mlock` next.

```
MLOCK(2)                                               Linux Programmer's Manual

NAME
       mlock, mlock2, munlock, mlockall, munlockall - lock and unlock memory
```

The lab includes two example programs using `mmap` to map a file into memory. Read the source, paying attention to what's happening (keep your `mmap` manpage open). [`save_array.c`](save_array.c) maps a file into memory as an array of numbers and populates the array. On the other hand, [`print_array.c`](print_array.c) maps the same file into memory and prints the numbers one by one. Compile them and run `./save_array` first, then `./print_array`. After running the first, a file should be appear, called `array.mem`. If you open the file, you will see some random looking characters. Try opening the file in hex mode with an appropriate program (try `man -k hex | grep dump`). One method is `hexdump array.mem`. If you look at the hex decoding of the contents, you might recognize the individual numbers written into the array by `save_array`.  See `man hexdump` for other switches to control the format of the output.  You may find one that is easier to read. 

Discuss with your neighbor: 

1. What do you think `mlock` is good for? This article discusses some of the trade-offs nicely: <https://eklitzke.org/mlock-and-mlockall>

2. How could you use `mmap` as a replacement for `malloc`: what flags would you use, how would you free the memory?

**Write a 1-2 sentence answer for each in the [quiz](https://northeastern.instructure.com/courses/119197/quizzes/451497).**

## Part 4 - The simplest memory allocator (sbrk.c)

We can actually write a very simple implementation of a memory allocator. The allocator below  gives us however many bytes of memory requested. 

```c
// sbrk.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void *mymalloc(size_t size) {
  void *ptr = sbrk(0);
  void *request = sbrk(size);
  if (request == (void *) -1) {
    return NULL;
  }

  return ptr;
}

void setupProgram() {
  // Memory leak! But why?
  mymalloc(2);
  mymalloc(4);
  mymalloc(8);
  mymalloc(16);
  mymalloc(32);
  mymalloc(64);
  mymalloc(128);
  mymalloc(256);
}

int main(int argc, char **argv) {

  setupProgram();

  return 0;
}
```

However, the problem with this allocator is that we do not have any notion over where that memory is. We also do not really know how much memory needs to be freed(or rather, could be reused rather than asking for more memory). This means we need some sort of data structure to keep track of our heap size.

**Discuss** with your neighbor: 

1. What data structures might you use to keep track of memory? Record at least one, but try to think of at least two in your discussion.

2. Do you think you will be able to use `valgrind` to monitor your custom memory allocator to detect potential memory leaks? 

**Write a 1-2 sentence answer for each in the [quiz](https://northeastern.instructure.com/courses/119197/quizzes/451497).** 

## Part 5 - strace

Create **sbrk.c** using the code from Part 4.  Compile sbrk.c using `gcc -g sbrk.c -o sbrk`.  Run `strace ./sbrk`

[strace](https://linux.die.net/man/1/strace) is a program that intercepts system calls. You should be able to directly see where `sbrk` for instance has been called. Take a few moments to read the man pages.

```
STRACE(1)                                                General Commands Manual

NAME
       strace - trace system calls and signals

DESCRIPTION
       In the simplest case strace runs the specified command until it exits.
       It intercepts and records the system calls which are called  by
       a  process  and  the  signals  which  are  received by a process.  The
       name of each system call, its arguments and its return value are printed
       on standard error or to the file specified with the -o option.
```

**Discuss** with your neighbor how many `sbrk` system calls you see (they will show up as 'brk'), and write your answer below.

**Write a 1-2 sentence answer in the [quiz](https://northeastern.instructure.com/courses/119197/quizzes/451497).**

## Deliverables

1. Complete the Lab 7 quiz on Canvas: <https://northeastern.instructure.com/courses/119197/quizzes/451497> 
2. Check in and commit [sbrk.c](./sbrk.c)  to your repository.
3. Submit your repo to Gradescope.

## Going Further

* Take a look at [malloc.c](https://code.woboq.org/userspace/glibc/malloc/malloc.c.html). You can skim around and read some of the comments to see how the libc library performs memory allocation!
* Another nice resource is the GNU C Allocator described in great detail here: <https://www.gnu.org/software/libc/manual/html_node/Memory-Allocation.html>
