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

    // We open a file as usual, creating it if it doesn't exist, truncating it
    // if it does. Access mode: rw-r--r--
    int fd = open("array.mem", O_CREAT | O_TRUNC | O_RDWR, 0644);
    syscall_ok(fd, "open");

    // Important: the file's length might be zero initially, so we make sure
    // it is exactly `size` bytes long. See `man 2 ftruncate`.
    syscall_ok(ftruncate(fd, size), "ftruncate");

    // We map the open file into memory and we get a pointer back. Basicall,
    // we use this mmap call as a malloc, except all the contents of the 
    // "allocated" memory get written to a file.
    int *xs = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    syscall_ok((long) xs, "mmap");

    // Compute the contents of the array.
    for (int i = 0; i < 10; ++i) {
        xs[i] = i * i;
    }
    
    // Unmap the memory. This is like calling `free`, but we need to specify 
    // the size.
    syscall_ok(munmap(xs, size), "munmap");

    // Close the file.
    syscall_ok(close(fd), "close");

    return 0;
}
