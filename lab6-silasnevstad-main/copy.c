/* A program for copying files implemented using POSIX I/O */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

/**
 * Check if the given status was an error. If so print the error message with
 * the given prefix and exit.
 */
void exit_if_error(int status, const char *prefix) {
  if (status == -1) {
    perror(prefix);
    exit(1);
  }
}

int main(int argc, char **argv) {
  char buffer[BUFFER_SIZE];
  int length;

  // If we don't have the right number of args, show usage information
  if (argc != 3) {
    printf("usage: %s <source> <destination>\n\n"
           "Note: destination must be a file\n", argv[0]);
    exit(1);
  }

  int src_fd, dest_fd;
  
  // Open source for reading
  src_fd = open(argv[1], O_RDONLY);
  exit_if_error(src_fd, "Could not open source for reading");

  // Create the destination file with default permissions
  dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
  exit_if_error(dest_fd, "Could not open destination for writing");

  // Copy the file, BUFFER_SIZE bytes at a time
  while ((length = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
    exit_if_error(length, "Could not read from source");
    exit_if_error(write(dest_fd, buffer, length), "Could not write to destination");
  }

  close(src_fd);
  close(dest_fd);

  return 0;
}
