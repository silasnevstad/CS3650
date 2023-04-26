#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>

/* Check if a syscall succeeded and exit with an error if not. */
void syscall_ok(int result, const char *call) {
    if (result == -1) {
        fprintf(stderr, "Syscall %s failed: ", call);
        perror(NULL);
        exit(1);
    }
}

int main(int argc, char **argv) {
    // mapping 10 ints - each int is 4 bytes - 10 * sizeof(int) 
    int size = 10 * sizeof(int); // = 40

    // We open the file as usual.
    int fd = open("array.mem", O_RDWR);
    syscall_ok(fd, "open");

    // Once it's open, we map the file into memory, we get a pointer to the
    // mmapped memory block.
    int *xs = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    syscall_ok((long) xs, "mmap");


    // We print the memory address of the array and its contents.
    printf("%p\n", xs);
    for (int i = 0; i < 10; ++i) {
        printf("%d: %d\n", i, xs[i]);
    }

    // We unmap the memory. This is like calling `free` on malloc'd memory.
    syscall_ok(munmap(xs, size), "munmap");

    // Close the file.
    syscall_ok(close(fd), "close");

    return 0;
}

